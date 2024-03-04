import speech_recognition as sr
import keyboard as kb
import yaml

print("Starting strategem recogition")
print("SpeechRecognition Version:", sr.__version__)

print("Loading config...")
with open('stratagem.yml', 'r') as file:
    stratagems = yaml.safe_load(file)



recog = sr.Recognizer()
mic = sr.Microphone()
strat_key = 'ctrl'

def format_command(command):
    return command.replace("-", " ").lower()

def interpret_stratagem(command):
    command = format_command(command)

    for stratagem in stratagems:
        if not stratagem["enabled"]:
            continue

        for trigger in stratagem["trigger"]:
            if command == format_command(trigger):
                print("Found match: {0}".format(stratagem["name"]))
                return True

def listen():
    while True:
        print("Listening...")
        with mic as source:
            audio = recog.listen(source)
    
        print("Input recieved, converting... ")
        try:
            command = recog.recognize_google(audio)
            print("Heard:", command)

            if interpret_stratagem(command):
                break
            else:
                print("{0} was not a valid stratagem.".format(command))

        except sr.UnknownValueError:
            print("Could not understand audio.")
        except sr.RequestError as e:
            print("Could not decode, recieved error: {0}".format(e))
        
        if not kb.is_pressed(strat_key):    # Break if strategem key no longer held
            print("Stratagem key not pressed, listening stopped.")
            break

kb.add_hotkey(strat_key, listen)

kb.wait('esc')
print("Exiting")