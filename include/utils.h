#define MAX_ANALOG_READ 1023.0

float power(float base, int exp)
{

  float ans = 1.0;

  for (int i = 0; i < exp; i++)
    ans *= base;

  return ans;
}

/**
 * @brief Round value to number of decimals
 * 
 * @param var Value to round
 * @param decimals how many decimals after ,
 * @return float 
 */
float round_x(float var, int decimals)
{
  // 37.66666 * 100 =3766.66
  // 3766.66 + .5 =3767.16    for rounding off value
  // then type cast to int so value is 3767
  // then divided by 100 so the value converted into 37.67

  int precision = power(10.0, decimals);

  float value = (int)(var * precision + .5);
  return (float)value / precision;
}

/**
 * @brief Normalize value between 0 and one
 * 
 * @param in the value to normalize
 * @param max maximum value (will equal to 1)
 * @return float 
 */
float normalize(float in, float max=MAX_ANALOG_READ)
{
  return 1.0 - round_x(in / max, 3);
}

/**
 * @brief Conversion to frequency range
 * 
 * @param val 
 * @return float 
 */
float toFreq(float val)
{
  return 10.0 + val * 500.0;
}

/**
 * @brief Print Usage stats
 * 
 */
void print_stats() {

    Serial.print("Proc = ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" (");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("),  Mem = ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" (");
    Serial.print(AudioMemoryUsageMax());
    Serial.println(")");
    
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
}