#include <cstdint>

// SDL interface
struct ButtonState
{
	bool down = false;
	bool pressed = false;
	bool released = false;
};

enum class Buttons : uint8_t
{
	A,B,
	UP, DOWN, LEFT, RIGHT,
	START, SELECT
};

class Joypad
{
public:
	void Reset();
	void ButtonDown(Buttons button);
	void ButtonUp(Buttons button);

	uint8_t GetInput();
	void Write(uint8_t val);

private:
	uint8_t reg;

	bool up, down, left, right;
	bool a, b;
	bool start, select;

	
};