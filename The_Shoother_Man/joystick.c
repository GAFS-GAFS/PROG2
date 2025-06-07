#include "joystick.h"

joystick *createJoystick()
{
    joystick *personagem = (joystick *)malloc(sizeof(joystick));

    if (!personagem)
    {
        return (NULL);
    }

    personagem->right = 0;
    personagem->left = 0;
    personagem->up = 0;
    personagem->down = 0;
    personagem->fire = 0;

    return (personagem);
}

void destroyJoystick(joystick *element)
{
    free(element);
}

void rightMove(joystick *element)
{
    element->right = element->right ^ 1;
}
void leftMove(joystick *element)
{
    element->left = element->left ^ 1;
}
void upMove(joystick *element)
{
    element->up = element->up ^ 1;
}
void downMove(joystick *element)
{
    element->down = element->down ^ 1;
}
void fire(joystick *element)
{
    element->fire = element->fire ^ 1;
}