#pragma once
#include <regex>
#include <vector>
#include "Chord.h"
#include <optional>

class Parser
{
public:
	explicit Parser(std::istream& inputStream)
		: mInputStream(inputStream)
	{
	}

	[[nodiscard]] unsigned GetBpm() const
	{
		std::string line;
		if (std::getline(mInputStream, line))
		{
			try
			{
				return std::stoi(line);
			}
			catch (const std::exception&)
			{
				throw std::runtime_error("Invalid BPM format");
			}
		}
		throw std::runtime_error("No BPM found");
	}

	[[nodiscard]] std::vector<Chord> GetChords()
	{
		std::vector<Chord> chords;
		std::string line;

		while (std::getline(mInputStream, line))
		{
			if (line == "END")
				break;

			Chord chord;
			const std::regex noteRegex(R"(([A-G]#?\d-?)|(-))");
			const auto notesBegin = std::sregex_iterator(line.begin(), line.end(), noteRegex);
			const auto notesEnd = std::sregex_iterator();

			size_t noteIndex = 0;
			for (auto it = notesBegin; it != notesEnd; ++it, ++noteIndex)
			{
				const auto& match = *it;
				const auto noteStr = match.str();
				chord.push_back(ParseNoteOrDim(noteStr, noteIndex));
			}

			if (!chord.empty())
			{
				chords.push_back(chord);
				m_previousChord = chord;
			}
		}

		return chords;
	}

private:
	[[nodiscard]] Note ParseNoteOrDim(const std::string& noteStr, size_t noteIndex) const
	{
		if (noteStr == "-")
		{
			if (m_previousChord.has_value() && noteIndex < m_previousChord->size())
			{
				auto previousNote = m_previousChord.value()[noteIndex];
				previousNote.dim = true;
				return previousNote;
			}
			throw std::runtime_error("Invalid '-' note: no previous note to repeat");
		}
		return ParseNote(noteStr);
	}

	static Note ParseNote(const std::string& noteStr)
	{
		Note note;
		const std::regex noteRegex(R"(([A-G])(#?)(\d)(-?))");
		std::smatch match;

		if (std::regex_match(noteStr, match, noteRegex))
		{
			const auto noteName = match[1].str();
			const auto sharp = match[2].str();
			const auto octaveStr = match[3].str();
			const auto dimStr = match[4].str();

			note.type = GetNoteType(noteName, sharp);
			const auto octave = std::stoi(octaveStr);
			if (octave < 0 || octave > 8)
			{
				throw std::runtime_error("Invalid octave");
			}
			note.octave = octave;
			note.dim = !dimStr.empty();
		}
		else
		{
			throw std::runtime_error("Invalid note format: " + noteStr);
		}

		return note;
	}

	static NoteType GetNoteType(const std::string& noteName, const std::string& sharp)
	{
		if (noteName == "A")
			return sharp.empty() ? A : Ad;
		if (noteName == "B")
			return B;
		if (noteName == "C")
			return sharp.empty() ? C : Cd;
		if (noteName == "D")
			return sharp.empty() ? D : Dd;
		if (noteName == "E")
			return E;
		if (noteName == "F")
			return sharp.empty() ? F : Fd;
		if (noteName == "G")
			return sharp.empty() ? G : Gd;
		throw std::runtime_error("Invalid note name: " + noteName);
	}

private:
	std::istream& mInputStream;
	std::optional<Chord> m_previousChord;
};
