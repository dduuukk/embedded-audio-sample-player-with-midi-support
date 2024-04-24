import numpy as np
from scipy.io.wavfile import read
import matplotlib.pyplot as plt

# Load left and right audio samples
def load_audio_file(file):
    # Load audio files
    sample_rate, audio = read(file)
    # Normalize audio data
    audio = audio / np.iinfo(audio.dtype).max

    # Split audio into left and right channels if necessary
    if len(audio.shape) > 1:
        audio_left = audio[:, 0]
        audio_right = audio[:, 1]
    else:
        audio_left = audio
        audio_right = audio

    # # display the entire audio file in a chart
    # plt.figure(figsize=(14, 5))
    # plt.plot(audio_left)
    # plt.title('Waveform of Audio File')
    # plt.xlabel('Time (samples)')
    # plt.ylabel('Amplitude')
    # plt.show()

    return audio_left, audio_right, sample_rate

# Find the clap based on a threshold
def find_clap(audio, threshold):
    # Find the index of the first sample that exceeds the threshold
    for i in range(len(audio)):
        if abs(audio[i]) > threshold:
            return i
    return 0

# Align the input and output samples
def align_samples(file1, file2, threshold):
    # Load audio files
    file1_left, file1_right, sample_rate = load_audio_file(file1)
    file2_left, file2_right, sample_rate = load_audio_file(file2)

    # Find the clap in both audio files
    clap1 = find_clap(file1_left, threshold)
    clap2 = find_clap(file2_left, threshold)
    
    # If the clap in one audio file occurs later than in the other, remove the beginning of the later audio file
    if clap1 > clap2:
        file1_left = file1_left[clap1 - clap2:]
        file1_right = file1_right[clap1 - clap2:]
    elif clap2 > clap1:
        file2_left = file2_left[clap2 - clap1:]
        file2_right = file2_right[clap2 - clap1:]
    
    # If one audio file is longer than the other after alignment, trim the end of the longer audio file
    min_length = min(len(file1_left), len(file2_left))
    file1_left = file1_left[:min_length]
    file1_right = file1_right[:min_length]
    file2_left = file2_left[:min_length]
    file2_right = file2_right[:min_length]

    return file1_left, file1_right, file2_left, file2_right, sample_rate
    
# Compare the two audio samples
def compare_samples(file1, file2, threshold):    
    # Align the audio files
    file1_left, file1_right, file2_left, file2_right, sample_rate = align_samples(file1, file2, threshold)

    # Plot the waveforms of the two audio files
    plt.figure(figsize=(14, 5))

    plt.subplot(2, 1, 1)
    plt.plot(file1_left)
    plt.title('Waveform of File 1')
    plt.xlabel('Time (samples)')
    plt.ylabel('Amplitude')

    plt.subplot(2, 1, 2)
    plt.plot(file2_left)
    plt.title('Waveform of File 2')
    plt.xlabel('Time (samples)')
    plt.ylabel('Amplitude')

    plt.show()

    # Calculate the difference between each sample
    diff_left = np.abs(file1_left - file2_left)
    diff_right = np.abs(file1_right - file2_right)

    # Calculate the accuracy percentage
    accuracy_threshold = 0.01  # You can adjust this value as needed
    accuracy_left = np.sum(diff_left < accuracy_threshold) / len(diff_left)
    accuracy_right = np.sum(diff_right < accuracy_threshold) / len(diff_right)

    return accuracy_left * 100, accuracy_right * 100

file1 = 'hellnaw.wav'
file2 = 'output2.wav'
accuracy_left_1, accuracy_right_1 = compare_samples(file1, file2, 0.05)
print(f'Accuracy of left channel: {accuracy_left_1:.2f}%')
print(f'Accuracy of right channel: {accuracy_right_1:.2f}%')




# # Example usage
# file1 = '/path/to/audio1.wav'
# file2 = '/path/to/audio2.wav'
# file1_left, file1_right, sample_rate = load_audio_file(file1)
# file2_left, file2_right, sample_rate = load_audio_file(file2)

