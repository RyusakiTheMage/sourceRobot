void BiBC4() {
	for(int i = 0; i < 4; i++) {
		playTone(  261,   30); wait1Msec( 300);  // Note(C4, Duration(Quarter))
		playTone(    0,   60); wait1Msec( 600);  // Note(Rest, Duration(Half))
		playTone(  233,   15); wait1Msec( 150);  // Note(A#3)
		playTone(  233,   15); wait1Msec( 150);  // Note(A#3)
		playTone(  233,   30); wait1Msec( 300);  // Note(A#3, Duration(Quarter))
		playTone(    0,   60); wait1Msec( 600);  // Note(Rest, Duration(Half))
		playTone(  174,   15); wait1Msec( 150);  // Note(F3)
		playTone(  174,   15); wait1Msec( 150);  // Note(F3)
		playTone(  174,   30); wait1Msec( 300);  // Note(F3, Duration(Quarter))
		if (!(i % 2)) {
			playTone(    0,  105); wait1Msec(1050);  // Note(Rest, Duration(Half))
			playTone(  311,   15); wait1Msec( 150);  // Note(D#4)
			playTone(  261,   15); wait1Msec( 150);  // Note(C4)
			playTone(  233,   15); wait1Msec( 150);  // Note(A#3)
			playTone(  195,   15); wait1Msec( 150);  // Note(G3)
			playTone(  195,   15); wait1Msec( 150);  // Note(G3)
			playTone(  174,   15); wait1Msec( 150);  // Note(F3)
			playTone(  164,   15); wait1Msec( 150);  // Note(E3)
		}
		else {
			playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Half))
			playTone(  195,   15); wait1Msec( 150);  // Note(G3)
			playTone(  164,   30); wait1Msec( 300);  // Note(E3, Duration(Quarter))
			playTone(  195,   15); wait1Msec( 150);  // Note(G3)
			playTone(  174,   30); wait1Msec( 300);  // Note(F3, Duration(Quarter))
			playTone(  195,   15); wait1Msec( 150);  // Note(G3)
			playTone(  184,   15); wait1Msec( 150);  // Note(F#3)
			playTone(    0,   15); wait1Msec( 150);  // Note(Rest)
			playTone(  195,   15); wait1Msec( 150);  // Note(G3, Duration(Quarter))
			playTone(  195,   30); wait1Msec( 300);  // Note(G3, Duration(Quarter))
		}
	}
}

void CrazyTown() {
	for(int i = 0; i < 2; i++) {
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(    0,  120); wait1Msec(1200);  // Note(Rest, Duration(3 Quarters)
		playTone(  220,   20); wait1Msec( 200);  // Note(A3)
		playTone(  220,   20); wait1Msec( 200);  // Note(A3)
		playTone(    0,   40); wait1Msec( 400);  // Note(Rest, Duration(Quarter))
		playTone(  164,   20); wait1Msec( 200);  // Note(E3)
		playTone(  164,   20); wait1Msec( 200);  // Note(E3)
		playTone(    0,   40); wait1Msec( 400);  // Note(Rest, Duration(Quarter))
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(    0,  120); wait1Msec(1200);  // Note(Rest, Duration(3 Quarters))
		playTone(  293,   20); wait1Msec( 200);  // Note(D4)
		playTone(  293,   20); wait1Msec( 200);  // Note(D4)
		playTone(    0,   40); wait1Msec( 400);  // Note(Rest, Duration(Quarter))
		playTone(  164,   20); wait1Msec( 200);  // Note(E3)
		playTone(  164,   20); wait1Msec( 200);  // Note(E3)
		playTone(    0,   40); wait1Msec( 400);  // Note(Rest, Duration(Quarter))
	}
	for(int i = 0; i < 3; i++) {
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  277,   20); wait1Msec( 200);  // Note(C#4)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  293,   20); wait1Msec( 200);  // Note(D4)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  277,   20); wait1Msec( 200);  // Note(C#4)
		playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
		playTone(  246,   20); wait1Msec( 200);  // Note(B3)
		playTone(  220,   20); wait1Msec( 200);  // Note(A3)
		playTone(  207,   20); wait1Msec( 200);  // Note(G#3)
		playTone(  220,   20); wait1Msec( 200);  // Note(A3)
		playTone(  246,   20); wait1Msec( 200);  // Note(B3)
		playTone(  220,   20); wait1Msec( 200);  // Note(A3)
		playTone(  207,   20); wait1Msec( 200);  // Note(G#3)
		playTone(  164,   20); wait1Msec( 200);  // Note(E3)
	}
	playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
	playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
	playTone(  277,   20); wait1Msec( 200);  // Note(C#4)
	playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
	playTone(  293,   20); wait1Msec( 200);  // Note(D4)
	playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
	playTone(  277,   20); wait1Msec( 200);  // Note(C#4)
	playTone(  184,   20); wait1Msec( 200);  // Note(F#3)
}


void EyeoftheTiger() {
	for(i = 0; i < 2; i++) {
		playTone(  587,   30); wait1Msec( 300);  // Note(D5, Duration(Eighth))
		playTone(    0,   90); wait1Msec( 900);  // Note(Rest, Duration(3 Eighths))
		playTone(  587,   15); wait1Msec( 150);  // Note(D5, Duration(16th))
		playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
		playTone(  523,   15); wait1Msec( 150);  // Note(C5, Duration(16th))
		playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
		playTone(  587,   30); wait1Msec( 300);  // Note(D5, Duration(Eighth))
		playTone(    0,  120); wait1Msec(1200);  // Note(Rest, Duration(Half))
		if (!i) {
			playTone(  587,   15); wait1Msec( 150);  // Note(D5, Duration(16th))
			playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
			playTone(  523,   15); wait1Msec( 150);  // Note(C5, Duration(16th))
			playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
			playTone(  587,   30); wait1Msec( 300);  // Note(D5, Duration(Eighth))
			playTone(    0,  120); wait1Msec(1200);  // Note(Rest, Duration(Half))
		}
		if (i) {
			playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
			playTone(  587,   30); wait1Msec( 300);  // Note(D5, Duration(Eighth))
			playTone(    0,   15); wait1Msec( 150);  // Note(Rest, Duration(16th))
			playTone(  523,   30); wait1Msec( 300);  // Note(C5, Duration(Eighth))
			playTone(  587,   30); wait1Msec( 300);  // Note(D5, Duration(Eighth))
			playTone(    0,   90); wait1Msec( 900);  // Note(Rest, Duration(3 Eighths))))
		}
		playTone(  587,   15); wait1Msec( 150);  // Note(D5, Duration(16th))
		playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
		playTone(  440,   15); wait1Msec( 150);  // Note(A4, Duration(16th))
		playTone(    0,   30); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
		playTone(  466,  120); wait1Msec(1200);	 // Note(A#4, Duration(Half))
	}
	playTone(  698,   24); wait1Msec( 268);  // Note(F, Duration(Eighth))
	playTone(    0,   12); wait1Msec( 134);  // Note(Rest, Duration(16th))
	playTone(    0,    6); wait1Msec(  67);  // Note(Rest, Duration(32th))
	playTone(  659,   24); wait1Msec( 268);  // Note(E, Duration(Eighth))
	playTone(  698,   24); wait1Msec( 268);  // Note(F, Duration(Eighth))
	playTone(  783,   24); wait1Msec( 268);  // Note(G, Duration(Eighth))
	playTone(    0,   12); wait1Msec( 134);  // Note(Rest, Duration(16th))
	playTone(    0,    6); wait1Msec(  67);  // Note(Rest, Duration(32th))
	playTone(  698,   24); wait1Msec( 268);  // Note(F, Duration(Eighth))
	playTone(  783,   24); wait1Msec( 268);  // Note(G, Duration(Eighth))
	playTone(  880,   96); wait1Msec(1071);  // Note(A, Duration(Half))
	playTone(    0,   48); wait1Msec( 536);  // Note(Rest)
	playTone(  587,   24); wait1Msec( 268);  // Note(D, Duration(Eighth))
	playTone(  523,   12); wait1Msec( 134);  // Note(C, Duration(16th))
	playTone(  587,   24); wait1Msec( 268);  // Note(D, Duration(Eighth))
	playTone(    0,   12); wait1Msec( 134);  // Note(Rest, Duration(16th))
	playTone(  523,   24); wait1Msec( 268);  // Note(C, Duration(Eighth))
	return;
}
