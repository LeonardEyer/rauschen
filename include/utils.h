#define MAX_ANALOG_READ 439.0

float power(float base, int exp)
{

  float ans = 1.0;

  for (int i = 0; i < exp; i++)
    ans *= base;

  return ans;
}

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

float normalize(float in)
{
  return 1.0 - round_x(in / MAX_ANALOG_READ, 3);
}

float toFreq(float val)
{
  return 10.0 + val * 500.0;
}

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
