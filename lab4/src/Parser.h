#pragma once
#include <regex>
#include <vector>
#include "Chord.h"
#include <optional>

class Parser
{
public:
	explicit Parser(std::istream& inputStream)
		: m_inputStream(inputStream)
	{
		// распарсить здесь (исправлено)
		ParseFile();
	}

	[[nodiscard]] unsigned GetBpm() const
	{
		return m_bpm;
	}

	[[nodiscard]] std::vector<Chord> GetChords() const
	{
		return m_chords;
	}

	[[nodiscard]] std::string GetType() const
	{
		return m_type;
	}

private:
	void ParseFile()
	{
		ParseBpm();
		ParseType();
		ParseChords();
	}

	void ParseBpm()
	{
		std::string line;
		if (std::getline(m_inputStream, line))
		{
			try
			{
				m_bpm = std::stoi(line);
				return;
			}
			catch (const std::exception&)
			{
				throw std::runtime_error("Invalid BPM format");
			}
		}
		throw std::runtime_error("No BPM found");
	}

	void ParseType()
	{
		std::string line;
		if (std::getline(m_inputStream, line))
		{
			m_type = line;
			return;
		}
		throw std::runtime_error("No type found");
	}

	void ParseChords()
	{
		std::string line;
		const std::regex noteRegex(R"(([A-G]#?\d-?[s,p,z,t]?)|(-))");

		while (std::getline(m_inputStream, line))
		{
			if (line == "END")
			{
				break;
			}

			std::erase_if(line, ::isspace);
			std::istringstream lineStream(line);
			std::string noteStr;
			Chord chord;
			size_t noteIndex = 0;
			while (std::getline(lineStream, noteStr, '|'))
			{
				chord.push_back(ParseNoteOrDim(noteStr, noteIndex));
				++noteIndex;
			}

			if (!chord.empty())
			{
				m_chords.push_back(chord);
				m_previousChord = chord;
			}
			else
			{
				m_chords.push_back(m_previousChord.value());
			}
		}
	}

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

	Note ParseNote(const std::string& noteStr) const
	{
		Note note;
		const std::regex noteRegex(R"(([A-G])(#?)(\d)(-?)([s,p,z,t]?))");
		std::smatch match;

		if (std::regex_match(noteStr, match, noteRegex))
		{
			const auto noteName = match[1].str();
			const auto sharp = match[2].str();
			const auto octaveStr = match[3].str();
			const auto dimStr = match[4].str();
			const auto type = match[5].str();

			note.type = GetNoteType(noteName, sharp);
			const auto octave = std::stoi(octaveStr);
			if (octave < 0 || octave > 8)
			{
				throw std::runtime_error("Invalid octave");
			}
			note.octave = octave;
			note.dim = !dimStr.empty();
			note.wave = type.empty() ? m_type : type;
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
	std::istream& m_inputStream;
	std::optional<Chord> m_previousChord;
	unsigned m_bpm = 0;
	std::vector<Chord> m_chords;
	std::string m_type;
};
