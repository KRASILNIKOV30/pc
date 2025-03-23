#pragma once
#include <string>
#include <vector>

enum NoteType
{
	A, Ad, B, C, Cd, D, Dd, E, F, Fd, G, Gd
};

struct Note
{
	NoteType type;
	int octave;
	bool dim = false;
	std::string wave;
};

using Chord = std::vector<Note>;