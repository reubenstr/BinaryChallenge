enum class Difficulty
{
  Easy,
  Medium,
  Hard
};

enum class State
{
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
    state = State::NewGameSetup;
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


const int timeMSAllowedToCapture = 5000;
const int newGameCountDownStart = 5;

const int splashDelayMS = 3000;
const int newGameDelayMS = 1000;

static const char *difficultyText[6] = {"Easy", "Medium", "Hard"};

const byte difficultyBitsMin[3] = {1, 2, 4};
const byte difficultyBitsMax[3] = {2, 4, 7};

int countBits(unsigned char byte)
{
    int count = 0;
    for(int i = 0; i < 8; i++)
        count += (byte >> i) & 0x01;
    return count;
}