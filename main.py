import speech_recognition as sr
import yaml

print("Starting strategem recogition")
print("SpeechRecognition Version:", sr.__version__)

recog = sr.Recognizer()
mic = sr.Microphone()

print("Listening...")
with mic as source:
    audio = recog.listen(source)

print(recog.recognize_google(audio))