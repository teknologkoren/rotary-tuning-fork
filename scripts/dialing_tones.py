import itertools
import wave
import struct
import math
import ffmpeg

SAMPLE_RATE = 48_000


def phase_to_amplitude(phase):
  # The factor 2**13 was selected so that
  #   - each sample fits in a signed 16-bit integer (could've been no larger than 2**15-1)
  #   - lowered a bit to avoid it being maximum volume
  return round(2**13 * math.sin(phase))


def write_tone(f, phase, frequency, duration):
  phase_step = 2 * math.pi * frequency / SAMPLE_RATE
  frame = bytearray()
  desired_n_frames = round(SAMPLE_RATE * duration)

  for i in itertools.count():
    phase += phase_step
    if phase > math.pi * 2:
      phase -= math.pi * 2
    amplitude = phase_to_amplitude(phase)

    # Interpret the amplitude as a signed short (int16), pack it in little-endian,
    # and get the two bytes.
    b1, b2 = struct.pack('<h', amplitude)

    frame.append(b1)  # Left channel first byte
    frame.append(b2)  # Left channel second byte

    if i >= desired_n_frames:
      if abs(amplitude) < 1:
        print(f"Got {i - desired_n_frames} frames extra.")
        break

  f.writeframes(frame)
  return phase


def write_silence(f, phase, duration):
  return write_tone(f, phase, 0, duration)


def write_signal(name, tones):
  with wave.open(f"wav/{name}.wav", "wb") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(SAMPLE_RATE)

    phase = 0.0
    for tone in tones:
      frequency = tone[0]
      duration = tone[1]
      phase = write_tone(f, phase, frequency, duration)
  
  stream = ffmpeg.input(f"wav/{name}.wav")
  stream = ffmpeg.output(stream, f"mp3/{name}.mp3", acodec="mp3", loglevel="quiet")
  ffmpeg.run(stream)


## Generate the tones.
# Source: https://www.etsi.org/deliver/etsi_tr/101000_101099/10104102/01.01.01_60/tr_10104102v010101p.pdf
# Generally, the Swedish variants have been chosen for maximal familiarity.

# Tone that plays while waiting for number input.
write_signal("dial", ((425, 5),))

# Tone that plays while the call is ringing. Do two periods so you can
# hear it properly.
write_signal("ringing", (
  (425, 1),
  (0, 4),
  (425, 1),
  (0, 4),
  ))

# Tone that plays if the number was busy. Do four periods so you can
# hear it properly.
write_signal("busy", (
  (425, 0.25),
  (0, 0.25),
  (425, 0.25),
  (0, 0.25),
  (425, 0.25),
  (0, 0.25),
  (425, 0.25),
  (0, 0.25),
  ))

# Standard busy signal (the Swedish one is twice as fast).
write_signal("busy_std", (
  (425, 0.5),
  (0, 0.5),
  (425, 0.5),
  (0, 0.5),
  (425, 0.5),
  (0, 0.5),
  (425, 0.5),
  (0, 0.5),
  ))

# Special information signal. Play it twice for good measure and demo purposes.
write_signal("special_information", (
  (950, 0.33),
  (1400, 0.33),
  (1800, 0.33),
  (0, 1),
  (950, 0.33),
  (1400, 0.33),
  (1800, 0.33),
  (0, 1),
  ))
