import speech_recognition as sr
import keyboard as kb
import yaml
import time

print("Starting strategem recogition")
print("SpeechRecognition Version: {0}".format(sr.__version__))

print("Loading config...")
with open('stratagem.yml', 'r') as file:
    stratagems = yaml.safe_load(file)

with open('config.yml', 'r') as file:
    config = yaml.safe_load(file)

strat_key = config["keys"]["stratagem"]

recog = sr.Recognizer()
mic = sr.Microphone()
print("Calibrating for ambient noise...")
with mic as source:
    recog.adjust_for_ambient_noise(source)

def format_command(command):
    return command.replace("-", " ").lower()

def execute_stratagem(stratagem):
    print("Executing {0}.".format(stratagem["name"]))
    for key in stratagem["code"]:
        if key == "U":
            kb.send(config["keys"]["up"])
        elif key == "D":
            kb.send(config["keys"]["down"])
        elif key == "L":
            kb.send(config["keys"]["left"])
        else:
            kb.send(config["keys"]["right"])
        
        # Ignore on last?
        time.sleep(config["dialling-speed"])


def interpret_stratagem(command):
    command = format_command(command)

    for stratagem in stratagems:
        if not stratagem["enabled"]:
            continue

        # Could be accomplished using a trigger map
        # Has not been done in yaml file for readability
        for trigger in stratagem["trigger"]:
            if command == format_command(trigger):
                print("Found match: {0}".format(stratagem["name"]))
                execute_stratagem(stratagem)
                return True
        
    return False

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
print("Ready")

kb.wait(config["keys"]["exit"])
print("Exiting")