enum class Difficulty
{
  Easy,
  Medium,
  Hard
};

enum class State
{
  Hold,
  NewGameSetup,  
  SelectDifficulty,
  Play,
  ResetToggles,
  EndOfGame,
  HighScores
};

struct Game
{
  Game()
  {
    state = State::Hold;
    difficulty = Difficulty::Easy;
  }

  State state;
  Difficulty difficulty;
  byte target;
  int turn;
  bool showDecValues;
  int scoreTotal;
  int score;
  unsigned long startTimeToCapture;
  int newGameCountDown;

  const int numTurns = 10;
};

enum class Tone
{
  Startup,
  NewGame,
  CaptureSuccess,
  CaptureFail,
  OutofTime,
  TogglesReset,
  EndOfGame,
};

enum class Display
{
  Splash, 
  SelectDifficulty,
  UpdateDifficulty,
  GameInfo,
  Target,
  Countdown,
  ResetTogglesOnly,
  ResetTogglesWithScore,
  FinalScore
};

const int timeMSAllowedToCapture = 5000;
const int newGameCountDownStart = 5;

const int splashDelayMS = 3000;

static const char *difficultyText[6] = {"Easy", "Medium", "Hard"};
static const char *difficultyTextUC[6] = {" EASY ", "MEDIUM", " HARD "};
const uint16_t difficultyColors[] = {TFT_GREEN, TFT_YELLOW, TFT_RED};

const byte difficultyBitsMin[3] = {1, 2, 4};
const byte difficultyBitsMax[3] = {2, 4, 7};