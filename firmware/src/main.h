enum class Difficulty
{
  Easy,
  Medium,
  Hard
};

enum class State
{
  Home,
  SetDifficulty,
  Play,
  ResetToggles,
  HighScores
};

enum class Tone
{
  Startup,
  NewGame,
  Capture,
  OutofTime,
  TogglesReset,
  EndOfGame,
};


static const char *difficultyText[6] = {"Easy", "Medium", "Hard"};