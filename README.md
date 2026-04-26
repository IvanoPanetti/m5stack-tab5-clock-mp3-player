# M5Stack Tab5 – Clock & MP3 Music Player  
*(Italiano sotto)*


---

## 🇬🇧 English Version

### **Overview**
This project turns the **M5Stack Tab5** into a combined **analog clock** and **MP3 music player**.  
It was created for fun, with the help of AI, and may contain bugs.  
Use it freely at your own risk.

🔄 Changelog – Latest Updates
Changed the font that caused a compilation error.

Fixed a timing bug in the screen‑off function.

The firmware folder now includes a pre‑compiled firmware.bin ready to be loaded using a launcher.

Added in the Audio Menu the option to set an alarm clock.

---

## 📁 **SD Card Setup**
- Copy the entire **SD** folder to the **root** of your microSD card.  
- The SD card **must be formatted as FAT32**, otherwise it will not be recognized.  
- The file **Setting.txt** contains all project settings.

### **Setting.txt format**

VariableName = Value ; Comment

The symbols `=` and `;` must be used exactly as separators.

---

## 🕒 **Analog Clock**
The analog clock display is inspired by the **Bodmer TFT_Clock** demo from the **TFT_eSPI** library.

---

## 📡 **Wi-Fi Menu**
From this menu you can select one of three Wi-Fi networks.  
These networks must be defined inside **Setting.txt** on the SD card.

Time synchronization can be done:
- manually (recommended), or  
- via Wi-Fi (unreliable, may require several attempts)

If synchronization fails, use manual input.

---

## 🎵 **Audio Menu**
- Songs must be placed inside dedicated SD folders.  
- Song filenames must follow this format:

ArtistName-SongTitle

Important:
- The `-` separator must be present.  
- No spaces near the `-` symbol.

There is no fixed limit to the number of songs.  
You can add as many as your SD card and Tab5 RAM allow.  
Tested with **over 500 songs per folder**.

### **Updating song lists**
Whenever you add or remove songs, run **Update Song List** from the menu.

---

## ⚠️ **Disclaimer**
This project was created for fun.  
I am not a programmer.  
It may contain bugs.  
Use it at your own risk.  
I am not responsible for any damage or inconvenience caused by this software.

License: **MIT**, unless otherwise specified by third‑party components.

🛠️ Planned Features
Web radio player (.mp3 / .aac codecs)

Alarm clock

🇮🇹 Versione Italiana
Descrizione
Questo progetto trasforma l’M5Stack Tab5 in un orologio analogico e lettore MP3.
È stato creato per divertimento, con il supporto dell’AI, e può contenere bug.
Usatelo liberamente a vostro rischio.

🔄 Registro Modifiche – Ultimi Aggiornamenti
Cambiato il font che causava un errore di compilazione.

Corretto un errore di temporizzazione nello spegnimento dello schermo.

La cartella firmware ora contiene il firmware.bin già compilato, pronto per essere caricato tramite un launcher.

Aggiunta nel Menu Audio la possibilità di impostare una sveglia.

---

## 📁 **Preparazione della SD**
- Copiare l’intera cartella **SD** nella **ROOT** della scheda microSD.  
- La scheda deve essere **formattata FAT32**, altrimenti non verrà riconosciuta.  
- Il file **Setting.txt** contiene tutte le impostazioni del progetto.

### **Formato di Setting.txt**

NomeVariabile = Valore ; Commento

I simboli `=` e `;` devono essere usati come separatori.

---

## 🕒 **Orologio Analogico**
La visualizzazione dell’orologio è ispirata alla demo **TFT_Clock** della libreria **TFT_eSPI** di Bodmer.

---

## 📡 **Menu Wi-Fi**
Da questo menu è possibile selezionare una delle tre reti Wi-Fi.  
Le reti devono essere inserite nel file **Setting.txt** nella root della SD.

La sincronizzazione dell’orario può essere:
- manuale (consigliata), oppure  
- tramite Wi-Fi (poco affidabile, richiede più tentativi)

In caso di problemi, usare l’inserimento manuale.

---

## 🎵 **Menu Audio**
- Le canzoni vanno inserite nelle cartelle dedicate sulla SD.   
- I nomi dei file devono essere nel formato:

NomeArtista-TitoloCanzone


Importante:
- Il simbolo `-` è obbligatorio.  
- Non devono esserci spazi vicino al trattino.

Il numero di canzoni non è definito.  
Potete inserirne quante la SD e la RAM del Tab5 permettono.  
Testato con **oltre 500 brani per cartella**.

### **Aggiornamento degli elenchi**
Dopo aver modificato le canzoni, usare l’opzione **Aggiorna Lista Canzoni**.

---

## ⚠️ **Avvertenze**
Questo progetto è stato creato per gioco.  
Non sono un programmatore.  
Può contenere bug.  
Usatelo a vostro rischio.  
Non sono responsabile di eventuali danni o disagi causati da questo software.

Licenza: **MIT**, salvo diversa indicazione nei componenti di terze parti.

🛠️ Prossimi Sviluppi
Player WebRadio (.mp3 / .aac)

AGGIORNATO 26/04/26