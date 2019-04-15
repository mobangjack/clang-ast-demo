#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct acc
{
    float ax;
    float ay;
    float az;
    char flag;
} acc_t;

typedef struct gyro
{
    float gx;
    float gy;
    float gz;
    char flag;
} gyro_t;

typedef struct imu
{
    acc_t acc;
    gyro_t gyro;
} imu_t;

#pragma pack()

#ifdef __cplusplus
}
#endif


