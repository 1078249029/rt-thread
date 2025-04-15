#include <rtthread.h>
#ifndef __KALMAN_CAL_H
#define __KALMAN_CAL_H

// 状态向量维度: [px, py, pz, vx, vy, vz, f1_ax, f1_ay, f1_az, f2_ax, f2_ay, f2_az]
#define STATE_DIM 12

// 观测维度: 两个MPU6050的6轴加速度 (ax1, ay1, az1, ax2, ay2, az2)
#define OBS_DIM 6

// 卡尔曼滤波结构体
typedef struct {
    float x[STATE_DIM];      // 状态向量
    float P[STATE_DIM][STATE_DIM]; // 协方差矩阵
    float F[STATE_DIM][STATE_DIM]; // 状态转移矩阵
    float H[OBS_DIM][STATE_DIM];   // 观测矩阵
    float Q[STATE_DIM][STATE_DIM]; // 过程噪声协方差
    float R[OBS_DIM][OBS_DIM];     // 观测噪声协方差
} KalmanFilter;

rt_base_t ktest();
void KF_Update(KalmanFilter *kf, float *z);
void KF_Predict(KalmanFilter *kf, float dt);

KalmanFilter kf = {0};
static float P_new[STATE_DIM][STATE_DIM] = {0};	// 下一次滤波的的协方差
static float KH[STATE_DIM][STATE_DIM];	// 中间值，K*H 更新协方差需要计算
static float I_KH[STATE_DIM][STATE_DIM] = {0};	// 中间值，I-KH 更新协方差需要计算
static float K[STATE_DIM][OBS_DIM] = {0};	// 卡尔曼系数矩阵
static float HT[STATE_DIM][OBS_DIM] = {0};	// 观测矩阵的转置(观测直接关联到零偏)
static float HP[OBS_DIM][STATE_DIM] = {0};	// 中间值，获取新的卡尔曼增益矩阵时需要计算
static float HPH[OBS_DIM][OBS_DIM] = {0};	// 中间值，获取新的卡尔曼增益矩阵时需要计算
static float HPH_inv[OBS_DIM][OBS_DIM] = {0};	// 中间值，获取新的卡尔曼增益矩阵时需要计算

#endif
