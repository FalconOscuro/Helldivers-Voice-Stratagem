# Helldivers Voice Stratagems
Functionality for voice activated stratagems within Helldivers 2.

I made this as a bit of fun for my friends and, as such, it is intentionally jank.

# Requirements

- Python 3.11 (Other versions may work, project was developed using 3.11.1)
- keyboard (Hotkey detection and macros)
- PyYaml (configuration files)
- SpeechRecognition
    - pocketsphinx (Speech recognition)
    - syllables (word syllable estimation at an attempt to improve accuracy)
    - PyAudio (microphone)
    - num2words (number conversion for recognition)
- PyQt5 (UI)

I would reccomend setting up a [python virtual environment](https://docs.python.org/3/library/venv.html) to install the required modules, which can be done using:

```pip install -r requirements.txt```

# Usage

The program can be run with the command `python ./hdvs/main.py` from within the project root directory. If run in place it will be unable to find the configuration files.

Once running the tool will await for the *stratagem key* to be pressed, and so long as it is held, it will listen until a stratagem trigger word is heard.
Once a trigger is identified the stratagem code is dialled and hopefully you ended up with the correct one.

> NOTE: Setting changes within the program do to take effect until apply is pressed

> NOTE2: Disabling unused stratagems may increase the accuracy of speech recogintion (alongside preventing unequipped stratagems from being attempted), but your mileage may vary.

# Disclaimer

I am not associated with Arrowhead Game Studios, this is an un-official tool, I do not believe it to be in breach of their EULA, but as with any random download on the internet use this at your own risk.

All icons used are sourced from [the helldivers wiki](https://helldivers.fandom.com/wiki/Stratagem_Codes_(Helldivers_2)) and are the property of Arrowhead Game Studios.

Glory to Super Earth and godspeed Helldivers.