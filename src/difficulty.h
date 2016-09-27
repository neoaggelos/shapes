#ifndef _difficulty_h
#define _difficulty_h

enum DifficultyLevel {
    Easy=1,
    Medium=2,
    Hard=3
};

class Difficulty {
private:
    DifficultyLevel level;
public:

    Difficulty(DifficultyLevel l) { level = l; }
    
    double shapeSpeed();
    Uint32 respawnTime();
	Uint32 changeModeTime();
    int score();
    int numShapes();

};

DifficultyLevel difficultyLevel(string level);
const char* difficultyName(int level);

#endif /* _difficulty_h */