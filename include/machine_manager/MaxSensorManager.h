#ifndef _included_max_sens_manager
#define _included_max_sens_manager
#include "config.h"
// #define _use_sparkfun 1

bool _max_is_run = false,
     valid_beat = false;

uint _max_beat_count = 0,
     _max_current_bpm = 0,
     _max_spo2 = 0,
     bpm;

// finger
// Timestamp for finger detection
long finger_timestamp = 0;
bool finger_detected = false;
const unsigned int finger_sensor_treshold = 25000;
const long detected_finger_dur_min_tres = 1000,
           no_finger_dur_max_tres = 1000;
long detected_finger_timestamp = 0,
     no_finger_timestamp = 0;

#ifndef _use_sparkfun
// Sensor (adjust to your sensor type)
#include <MAX3010x.h>
#include "filters.h"
MAX30102 sensor;
const auto kSamplingRate = sensor.SAMPLING_RATE_800SPS;
const float kSamplingFrequency = 400.0;

// Finger Detection Threshold and Cooldown
const unsigned long kFingerThreshold = 10000;
const unsigned int kFingerCooldownMs = 500;

// Edge Detection Threshold (decrease for MAX30100)
const float kEdgeThreshold = -500.0;

// Filters
const float kLowPassCutoff = 4.0;
const float kHighPassCutoff = 0.5;

// Averaging
const bool kEnableAveraging = false;
const int kAveragingSamples = 10;
const int kSampleThreshold = 7;

// Filter Instances
LowPassFilter low_pass_filter_red(3.5, kSamplingFrequency);
LowPassFilter low_pass_filter_ir(kLowPassCutoff, kSamplingFrequency);
LowPassFilter low_pass_diff(3, kSamplingFrequency);
HighPassFilter high_pass_filter_red(kHighPassCutoff, kSamplingFrequency);
Differentiator differentiator(kSamplingFrequency);
MovingAverageFilter<kAveragingSamples> averager_bpm;
MovingAverageFilter<kAveragingSamples> averager_r;
MovingAverageFilter<kAveragingSamples> averager_spo2;
MovingAverageFilter<50> averager_raw_red;
MovingAverageFilter<70> averager_diff;

// Statistic for pulse oximetry
MinMaxAvgStatistic stat_red;
MinMaxAvgStatistic stat_ir;

// R value to SpO2 calibration factors
// See https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html
float kSpO2_A = 1.2;
float kSpO2_B = -9.6596622;
float kSpO2_C = 112.6898759;

// Timestamp of the last heartbeat
long last_heartbeat = 0;

// Last diff to detect zero crossing
float last_diff = NAN;
bool crossed = false;
long crossed_time = 0;

bool max_init()
{
    if (sensor.begin() && sensor.setSamplingRate(kSamplingRate))
    {
#if _MAX_MANAGER_DEBUG
        Serial.println("Max30102 initialized");
#endif
        return true;
    }
#if _MAX_MANAGER_DEBUG
    Serial.println("Failed initializing Max30102");
#endif
    return false;
}
void max_runner()
{
    // apakah max perlu dijalankan?
    if (!_max_is_run)
        return;

    // baca nilai pada sensor
    auto sample = sensor.readSample(1000);

    // periksa apakah nilai valid
    if (!sample.valid)
        return;

    // pisahkan nilai
    float current_value_red = sample.red;
    float current_value_ir = sample.ir;

    // periksa apakah nilai ir atau red tidak melewati batas
    if (current_value_ir > 1000000 || current_value_red > 10000000)
        return;

    // periksa apkah nilai ir memenuhi syarat jari terdeteksi
    if (current_value_ir > finger_sensor_treshold)
    {
        detected_finger_timestamp = millis();
    }

    if (current_value_ir < finger_sensor_treshold)
    {
        no_finger_timestamp = millis();
    }

    // memeriksa apakah jari sudah ada selama waktu minimal yang ditentukan
    if ((long)(detected_finger_timestamp - no_finger_timestamp) > (long)detected_finger_dur_min_tres)
        finger_detected = true;
    else if ((long)(no_finger_timestamp - detected_finger_timestamp) > (long)no_finger_dur_max_tres)
        finger_detected = false;

    // reset semua nilai saat tidak ada jari
    if (!finger_detected)
    {
        // Reset values if the finger is removed
        differentiator.reset();
        averager_bpm.reset();
        averager_r.reset();
        averager_spo2.reset();
        low_pass_filter_red.reset();
        low_pass_filter_ir.reset();
        high_pass_filter_red.reset();
        stat_red.reset();
        stat_ir.reset();
        averager_raw_red.reset();
        low_pass_diff.reset();
        averager_diff.reset();

        finger_detected = false;
        finger_timestamp = millis();
    }

    // lakukan perhitungan jika ada jari
    if (finger_detected)
    {
        // membersihkan sinyal merah
        float red_lo_pass = low_pass_filter_red.process(current_value_red);
        float red_hi_pass = high_pass_filter_red.process(red_lo_pass);
        float red_avg = averager_raw_red.process(red_hi_pass);

        // membersihkan sinyal inframerah
        current_value_ir = low_pass_filter_ir.process(current_value_ir);

        // masukkan nilai inframerah dan merah ke prosesor statistik
        stat_red.process(red_lo_pass);
        stat_ir.process(current_value_ir);

        // memeriksa keberadaan detak jantung dari cahaya merah
        float current_value = red_avg;
        float current_diff = differentiator.process(current_value);
        // current_diff = averager_diff.process(current_diff);

        // Valid values?
        if (!isnan(current_diff) && !isnan(last_diff))
        {
            // Detect Heartbeat - Zero-Crossing
            if (last_diff > 0 && current_diff < 0)
            {
                crossed = true;
                crossed_time = millis();
            }

            if (current_diff > 0)
            {
                crossed = false;
            }

            // Serial.printf("current_diff: %f\n", current_diff);
            // Detect Heartbeat - Falling Edge Threshold
            if (crossed && current_diff < kEdgeThreshold)
            {
                if (last_heartbeat != 0 && crossed_time - last_heartbeat > 300)
                {
                    valid_beat = true;
                    // DETAK JANTUNG VALID
                    // heartrate calc
                    int bpm = 60000 / (crossed_time - last_heartbeat);
                    _max_beat_count++;
                    _max_current_bpm = averager_bpm.process(bpm);

                    // spo2 calc
                    float rred = (stat_red.maximum() - stat_red.minimum()) / stat_red.average();
                    float rir = (stat_ir.maximum() - stat_ir.minimum()) / stat_ir.average();
                    float r = rred / rir;
                    float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;
                    if (spo2 > 100)
                        spo2 = 100;
                    if (bpm > 50 && bpm < 250)
                    {
                        int average_r = averager_r.process(r);
                        int average_spo2 = averager_spo2.process(spo2);
                        _max_spo2 = average_spo2;
                    }

                    // Reset statistic
                    stat_red.reset();
                    stat_ir.reset();
                }

                crossed = false;
                last_heartbeat = crossed_time;
            }
            else
            {
                valid_beat = false;
            }
        }

#if _MAX_MANAGER_DEBUG
        Serial.printf("curr_val, last_dif,  cur_dif, beat\n%f, %f, %f, %d\n",
                      current_value, last_diff, current_diff, valid_beat ? 100 : -100);
#endif
        last_diff = current_diff;
    }
}
#endif

void max_start_read()
{
#if _MAX_MANAGER_DEBUG
    Serial.println("MAX: started heartrate counting");
#endif
    _max_is_run = true;
}

void max_stop_read()
{
#if _MAX_MANAGER_DEBUG
    Serial.println("MAX: stopped heartrate counting");
#endif
    _max_is_run = false;
    _max_beat_count = 0;
    _max_current_bpm = 0;
    _max_spo2 = 0;
}

uint max_get_beat_count() { return _max_beat_count; }
uint max_get_current_bpm() { return _max_current_bpm; }
uint max_get_spo2() { return _max_spo2; }
#endif
