#include "kalman.h"
#include "rttypes.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>

// 初始化卡尔曼滤波器
void KF_Init(KalmanFilter *kf, float dt) {
    // 初始化状态为0
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        kf->x[i] = 0.0f;
    }

    // 初始化协方差矩阵（较大的初始不确定性）
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            kf->P[i][j] = (i == j) ? 100.0f : 0.0f;
        }
    }

    // 状态转移矩阵 F (线性运动模型)
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            kf->F[i][j] = 0.0f;
        }
    }
    // 位置和速度关系 (p_k+1 = p_k + v_k*dt + 0.5*a*dt^2 忽略输入加速度项)
    for (rt_base_t i = 0; i < 3; i++) {
        kf->F[i][i] = 1.0f;          // 位置
        kf->F[i][i+3] = dt;          // 速度 -> 位置
    }
    // 速度关系 (v_k+1 = v_k)
    for (rt_base_t i = 3; i < 6; i++) {
        kf->F[i][i] = 1.0f;
    }
    // 零偏建模为随机游走 (b_k+1 = b_k)，这意味着零偏恒定
    for (rt_base_t i = 6; i < STATE_DIM; i++) {
        kf->F[i][i] = 1.0f;
    }
	
	// *关键改动*
    // 观测矩阵 H (观测直接关联到零偏)
    // 示例：观测值 z = [m1x, m1y, m1z, m2x, m2y, m2z]
	float H[6][12] = {
		{0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0}, // m1x = ax + f1_ax_b
		{0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, // m1y = ay + f1_ay_b
		{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0}, // m1z = az + f1_az_b
		{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0}, // m2x = ax + f2_ax_b
		{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}, // m2y = ay + f2_ay_b
		{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1}  // m2z = az + f2_az_b
	};

	// static float Q_angle = 0.001;		//陀螺仪角度数据置信度，角度噪声的协方差，由冲击、振动影响，是系统特性
	// static float Q_gyro  = 0.003;		//陀螺仪角速度数据置信度，角速度噪声的协方差，由冲击、振动影响，是系统特性
	// static float R_angle = 0.5;		//加速度计测量噪声的协方差，越大越不信任加速度计，由冲击、振动影响，是系统特性
    // 过程噪声协方差 Q (调整参数)
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            kf->Q[i][j] = (i == j) ? 0.075f : 0.0f; // 对角矩阵，过程噪声矩阵Q过小会导致测量值滞后真实值
        }
    }

    // 观测噪声协方差 R (根据传感器噪声特性调整)
    for (rt_base_t i = 0; i < OBS_DIM; i++) {
        for (rt_base_t j = 0; j < OBS_DIM; j++) {
            kf->R[i][j] = (i == j) ? 0.3f : 0.0f; // 假设各轴独立，测量噪声矩阵R过大会导致测量值滞后真实值
        }
    }
}

// 矩阵乘法: C = A * B (A: m×n, B: n×p, C: m×p)
void mat_mult(float *A, float *B, float *C, rt_base_t m, rt_base_t n, rt_base_t p) {
    float *tmp = (float *)malloc(m * p * sizeof(float)); // 临时存储结果
    for (rt_base_t i = 0; i < m; i++) {
        for (rt_base_t j = 0; j < p; j++) {
            tmp[i*p + j] = 0;
            for (rt_base_t k = 0; k < n; k++) {
                tmp[i*p + j] += A[i*n + k] * B[k*p + j];
            }
        }
    }
    memcpy(C, tmp, m * p * sizeof(float)); // 拷贝回输出
    free(tmp);
}


// 矩阵转置: AT = A^T (A: m×n, AT: n×m)
void mat_transpose(float *A, float *AT, rt_base_t m, rt_base_t n) {
    for (rt_base_t i = 0; i < m; i++) {
        for (rt_base_t j = 0; j < n; j++) {
            AT[j*m + i] = A[i*n + j];
        }
    }
}

// 交换两行
static void swap_rows(float *A, rt_base_t n, rt_base_t row1, rt_base_t row2) {
    for (rt_base_t j = 0; j < n; j++) {
        float tmp = A[row1 * n + j];
        A[row1 * n + j] = A[row2 * n + j];
        A[row2 * n + j] = tmp;
    }
}
// 矩阵求逆 (LU分解部分选主元)
rt_base_t mat_inv(float *A, float *A_inv, rt_base_t n) {
    float LU[n * n];
    rt_base_t pivot[n];
    
    // 初始化A_inv为单位矩阵
    for (rt_base_t i = 0; i < n; i++) {
        for (rt_base_t j = 0; j < n; j++) {
            A_inv[i * n + j] = (i == j) ? 1.0f : 0.0f;
            LU[i * n + j] = A[i * n + j];
        }
        pivot[i] = i;
    }
    // LU分解（带部分选主元）
    for (rt_base_t k = 0; k < n; k++) {
        // 选主元
        rt_base_t max_row = k;
        float max_val = fabs(LU[k * n + k]);
        for (rt_base_t i = k + 1; i < n; i++) {
            if (fabs(LU[i * n + k]) > max_val) {
                max_row = i;
                max_val = fabs(LU[i * n + k]);
            }
        }
        if (max_val < 1e-12f) return -1; // 矩阵奇异，求逆失败
        // 交换行
        if (max_row != k) {
            swap_rows(LU, n, k, max_row);
            swap_rows(A_inv, n, k, max_row);
            rt_base_t tmp = pivot[k];
            pivot[k] = pivot[max_row];
            pivot[max_row] = tmp;
        }
        // 消元
        for (rt_base_t i = k + 1; i < n; i++) {
            LU[i * n + k] /= LU[k * n + k];
            for (rt_base_t j = k + 1; j < n; j++) {
                LU[i * n + j] -= LU[i * n + k] * LU[k * n + j];
            }
        }
    }
    // 解 L*y = I 和 U*x = y
    for (rt_base_t col = 0; col < n; col++) {
        // 前代 (L*y = I)
        for (rt_base_t i = 0; i < n; i++) {
            for (rt_base_t j = 0; j < i; j++) {
                A_inv[i * n + col] -= LU[i * n + j] * A_inv[j * n + col];
            }
        }
        // 回代 (U*x = y)
        for (rt_base_t i = n - 1; i >= 0; i--) {
            for (rt_base_t j = i + 1; j < n; j++) {
                A_inv[i * n + col] -= LU[i * n + j] * A_inv[j * n + col];
            }
            A_inv[i * n + col] /= LU[i * n + i];
        }
    }
    return 0; // 成功
}

// 卡尔曼预测步骤
void KF_Predict(KalmanFilter *kf, float dt) {
	
	// static float dt      = 0.01;		//采样周期即计算任务周期10ms
    // 更新状态转移矩阵中的时间相关项
    for (rt_base_t i = 0; i < 3; i++) {
        kf->F[i][i+3] = dt;   // 位置 = 位置 + 速度*dt
        kf->F[i+3][i+6] = dt; // 速度 = 速度 + 加速度*dt（假设加速度是控制输入或通过观测间接更新）
    }

    // 预测状态: x = F * x
    float x_pred[STATE_DIM];
    mat_mult((float *)kf->F, kf->x, x_pred, STATE_DIM, STATE_DIM, 1);
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        kf->x[i] = x_pred[i];
    }

    // 预测协方差: P = F * P * F^T + Q
    float FP[STATE_DIM][STATE_DIM], FT[STATE_DIM][STATE_DIM];
    mat_mult((float *)kf->F, (float *)kf->P, (float *)FP, STATE_DIM, STATE_DIM, STATE_DIM);
    mat_transpose((float *)kf->F, (float *)FT, STATE_DIM, STATE_DIM);
    mat_mult((float *)FP, (float *)FT, (float *)kf->P, STATE_DIM, STATE_DIM, STATE_DIM);
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            kf->P[i][j] += kf->Q[i][j];
        }
    }
}

// 卡尔曼更新步骤
void KF_Update(KalmanFilter *kf, float *z) {

    // 计算残差: y = z - H * x
    float Hx[OBS_DIM];
    mat_mult((float *)kf->H, kf->x, Hx, OBS_DIM, STATE_DIM, 1);
    float y[OBS_DIM];
    for (rt_base_t i = 0; i < OBS_DIM; i++) {
        y[i] = z[i] - Hx[i];
    }
	char h[10] = {0};

    // 计算卡尔曼增益: K = P * H^T * (H * P * H^T + R)^-1
    mat_transpose((float *)kf->H, (float *)HT, OBS_DIM, STATE_DIM);
    mat_mult((float *)kf->H, (float *)kf->P, (float *)HP, OBS_DIM, STATE_DIM, STATE_DIM);
    mat_mult((float *)HP, (float *)HT, (float *)HPH, OBS_DIM, STATE_DIM, OBS_DIM);
    for (rt_base_t i = 0; i < OBS_DIM; i++) {
        for (rt_base_t j = 0; j < OBS_DIM; j++) {
            HPH[i][j] += kf->R[i][j];
        }
    }
    mat_inv((float *)HPH, (float *)HPH_inv, OBS_DIM);
    mat_mult((float *)kf->P, (float *)HT, (float *)K, STATE_DIM, STATE_DIM, OBS_DIM);
    mat_mult((float *)K, (float *)HPH_inv, (float *)K, STATE_DIM, OBS_DIM, OBS_DIM);

    // 更新状态: x = x + K * y
    float Ky[STATE_DIM];
    mat_mult((float *)K, y, Ky, STATE_DIM, OBS_DIM, 1);
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        kf->x[i] += Ky[i];
    }

    // 更新协方差: P = (I - K * H) * P
    mat_mult((float *)K, (float *)kf->H, (float *)KH, STATE_DIM, OBS_DIM, STATE_DIM);
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            I_KH[i][j] = (i == j) ? 1.0f - KH[i][j] : -KH[i][j];
        }
    }
    mat_mult((float *)I_KH, (float *)kf->P, (float *)P_new, STATE_DIM, STATE_DIM, STATE_DIM);
    for (rt_base_t i = 0; i < STATE_DIM; i++) {
        for (rt_base_t j = 0; j < STATE_DIM; j++) {
            kf->P[i][j] = P_new[i][j];
        }
    }
}

// 生成高斯随机数（均值=0，标准差=sigma）
float gaussian_noise(float sigma) {
    float u1 = rand() / (float)RAND_MAX; // [0,1] 均匀分布
    float u2 = rand() / (float)RAND_MAX;
    return sigma * sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2); // Box-Muller变换
}

// 主函数示例
rt_base_t ktest() {
	
    float dt = 0.01f; // 时间步长 (10ms)
    rt_uint8_t i = 0;
    char axis_status[12][10];
    KF_Init(&kf, dt);
    srand(time(NULL)); //随机数种子
    // 模拟两个MPU6050的观测数据 (ax1,ay1,az1, ax2,ay2,az2)
    float m[OBS_DIM] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // 示例数据

    // 运行滤波
    for (rt_base_t step = 0; step < 100; step++) {
		m[0] = 0.5f * sinf(step * 0.1f) + gaussian_noise(0.1f); // 噪声标准差=0.1
        m[3] = 0.3f * sinf(step * 0.1f) + gaussian_noise(0.1f);

        KF_Predict(&kf, dt);
        KF_Update(&kf, m);
		
        //ma: m1x, m1y, m1z, m2x, m2y, m2z，原始测量值，没有滤波
        char ma[6][10] = {0};
        for (i = 0; i < 6; i++) {
            sprintf(&ma[i][0],"%.3f",m[i]);
        }

        //axis_status: px, py, pz, vx, vy, vz, mpu6050_1_ax, mpu6050_1_ay, mpu6050_1_az, mpu6050_2_ax, mpu6050_2_ay, mpu6050_2_az
        for(i = 0; i < 4; i++)
        {
            sprintf(&axis_status[i][0],"%.3f",kf.x[i]);
            sprintf(&axis_status[i+1][0],"%.3f",kf.x[i+1]);
            sprintf(&axis_status[i+2][0],"%.3f",kf.x[i+2]);
        }
        
		// 打印结果
		// m1x代表第1个mpu6050的x轴加速度的观测数据，此时并未滤波
		// f1_ax代表第1个传感器的x轴方向加速度的滤波数据
		// 由于传感器并不直接测量速度与位置，因此这些数据是根据滤波后的结果积分得到，并且*仅使用第一个传感器的加速度数据*
		rt_kprintf("Step %d: m1x=%s, m1y=%s, m1z=%s, m2x=%s, m2y=%s, m2z=%s\n",
			step, ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);

		rt_kprintf("Step %d: px=%s, vx=%s, f1_ax=%s, f2_ax=%s\n",
			step, axis_status[0], axis_status[3], axis_status[6], axis_status[9]);

		rt_kprintf("Step %d: py=%s, vy=%s, f1_ay=%s, f2_ay=%s\n",
			step, axis_status[1], axis_status[4], axis_status[7], axis_status[10]);
		
		rt_kprintf("Step %d: pz=%s, vz=%s, f1_az=%s, f2_az=%s\n",
			step, axis_status[2], axis_status[5], axis_status[8], axis_status[11]);
	}

    return 0;
}
MSH_CMD_EXPORT(ktest, ktest);