# Helldivers Voice Stratagems
Functionality for voice activated stratagems within Helldivers 2.

# Requirements
This tool requires the following python modules:
| Module | Usage |
| --- | --- |
| keyboard | keyboard input and output |
| PyYAML | configuration files |
| SpeechRecognition | speech to text |
| PyAudio | Microphone input |

These can be installed manually with:

```pip install <module name>```

Or automatically from the `requirements.txt` with:

```pip install -r requirements.txt```

> NOTE: I would advise checking the contents of `requirements.txt` before running this command and comparing against the above table before running this command.

# Usage
The program will wait until the stratagem key is pressed, then will attempt to execute the input for the defined stratagem from interpreted microphone input.