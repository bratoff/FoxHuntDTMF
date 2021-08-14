//Melody 1: Twinkle
  //Number of notes in the melody:
  const int melody_Twinkle[] PROGMEM = {
    NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4};
  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  const char durations_Twinkle[] PROGMEM = {2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
  const int notes_Twinkle = sizeof(durations_Twinkle);

//Melody 2: Zelda
  //Number of notes in the melody:
  const int melody_Zelda[] PROGMEM = {  //Define the melody as being the notes following using those defined in pitches.h
    NOTE_E4, NOTE_G4, NOTE_D4, NOTE_C4, NOTE_D4, 
    NOTE_E4, NOTE_G4, NOTE_D4,   0,     NOTE_E4, 
    NOTE_G4, NOTE_D5, NOTE_C5, NOTE_G4, NOTE_F4, 
    NOTE_E4, NOTE_D4,    0,    NOTE_E4, NOTE_G4, 
    NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4, 
    NOTE_D4,    0,    NOTE_E4, NOTE_G4, NOTE_D5, 
    NOTE_C5, NOTE_G4};
  const char durations_Zelda[] PROGMEM = {                              //Define the note durations, 1 to 1 with melody    1 = 8 beats 
    2,4,2,8,8,                                                          //                                                 2 = 4 beats (whole note)
    2,4,2,4,2,                                                          //                                                 4 = 2 beats (half note)
    4,2,4,2,8,                                                          //                                                 8 = 1 beats (quarter note)
    8,2,4,2,4,
    2,8,8,2,4,
    2,4,2,4,2,
    4,1 };
  const int notes_Zelda = sizeof(durations_Zelda);    

//Melody 5: Star Wars
  const int melody_StarWars[] PROGMEM = {
     NOTE_D5, NOTE_D5, NOTE_D5, NOTE_G5, NOTE_D6,
     NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G6, NOTE_D6,
     NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G6, NOTE_D6,
     NOTE_C6, NOTE_B5, NOTE_C6, NOTE_A5, NOTE_D5, 
     NOTE_D5, NOTE_G5, NOTE_D6, NOTE_C6, NOTE_B5, 
     NOTE_A5, NOTE_G6, NOTE_D6, NOTE_C6, NOTE_B5, 
     NOTE_A5, NOTE_G6, NOTE_D6, NOTE_C6, NOTE_B5, 
     NOTE_C6, NOTE_A5};
  const char durations_StarWars[] PROGMEM = {
      10, 10, 10, 2, 2, 
      10, 10, 10,  2,  4,
      10, 10, 10,  2,  4,
      10, 10, 10,  2,  4, 
       8,  2,  2, 10, 10, 
      10,  2,  4, 10, 10, 
      10,  2,  4, 10, 10, 
      10,  2};
  const int notes_StarWars = sizeof(durations_StarWars);
  
