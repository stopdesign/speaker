
#include "AudioTools.h"
#include "AudioLibs/A2DPStream.h"


Pipeline pipeline;
A2DPStream in;
I2SStream i2s;
Equilizer3Bands eq(i2s);
ConfigEquilizer3Bands cfg_eq;
FilteredStream<int16_t, float> filtered;


// AudioEffectStream effects(in);
// ADSRGain adsr(0.001, 0.001, 0.5, 0.005, 1.0);
// Compressor cmp(44100, 30, 20, 10, 10, 0.5);
// Delay dly(998, 0.5, 1.0, 44100);



StreamCopy copier(pipeline, in);


// Low Pass 4410 Hz
float coef_low[] = {
  0.003764322734880946,
  0.003847531547725393,
  -0.000000000000000002,
  -0.008431910568784987,
  -0.019015258426618890,
  -0.025939987455861957,
  -0.021638469542556577,
  0.000000000000000006,
  0.040139933597423398,
  0.093003884531468101,
  0.146676588881349101,
  0.186776049847971998,
  0.201634629706006957,
  0.186776049847971998,
  0.146676588881349101,
  0.093003884531468101,
  0.040139933597423398,
  0.000000000000000006,
  -0.021638469542556577,
  -0.025939987455861957,
  -0.019015258426618890,
  -0.008431910568784987,
  -0.000000000000000002,
  0.003847531547725391,
  0.003764322734880946,
};


// High Pass 4410
float coef_high[] = {
  -0.003764322734880946,
  -0.003847531547725393,
  0.000000000000000002,
  0.008431910568784987,
  0.019015258426618890,
  0.025939987455861957,
  0.021638469542556577,
  -0.000000000000000006,
  -0.040139933597423398,
  -0.093003884531468101,
  -0.146676588881349101,
  -0.186776049847971998,
  0.798365370293993015,
  -0.186776049847971998,
  -0.146676588881349101,
  -0.093003884531468101,
  -0.040139933597423398,
  -0.000000000000000006,
  0.021638469542556577,
  0.025939987455861957,
  0.019015258426618890,
  0.008431910568784987,
  0.000000000000000002,
  -0.003847531547725391,
  -0.003764322734880946,
};


AudioInfo info(44100, 2, 16);


// Arduino Setup
void setup(void) {
  Serial.begin(115200);
  delay(100);

  // start the bluetooth audio receiver
  Serial.println(F("starting A2DP..."));

  AudioLogger::instance().begin(Serial, AudioLogger::Warning);


  // setup output
  auto cfg_I2S = i2s.defaultConfig(TX_MODE);
  cfg_I2S.pin_ws = 2;
  cfg_I2S.pin_bck = 3;
  cfg_I2S.pin_data = 4;
  // cfg_I2S.buffer_size = 1024;
  i2s.begin(cfg_I2S);


  auto cfg = in.defaultConfig(RX_MODE);
  cfg.name = "MyReceiver";
  in.begin(cfg);

  // Update sample rate
  info.sample_rate = in.audioInfo().sample_rate;


  // // setup effects
  // Serial.println(F("Setup Effects"));
  // effects.addEffect(dly);
  // effects.begin(info);


  cfg_eq = eq.defaultConfig();
  // cfg_eq.setAudioInfo(cfg_I2S);  // use settings

  cfg_eq.gain_low = 1.0;
  cfg_eq.gain_medium = 0.50;
  cfg_eq.gain_high = 0.80;

  cfg_eq.freq_low = 400;
  cfg_eq.freq_high = 6000;

  eq.begin(cfg_eq);


  filtered.begin(info);
  filtered.setFilter(0, new FIR<float>(coef_low));   // Driver
  filtered.setFilter(1, new FIR<float>(coef_high));  // Tweeter

  // setup pipeline
  // pipeline.add(filtered);
  pipeline.setOutput(eq);
  // pipeline.setOutput(effects);

  // start all components with their default values
  pipeline.begin();
}

// Arduino loop
void loop() {
  copier.copy();
}