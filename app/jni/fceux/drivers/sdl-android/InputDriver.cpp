#include "InputDriver.h"
#include "../../git.h"

void UpdateInput (Config * config)
{
    char buf[64];
    std::string device, prefix;

    for (unsigned int i = 0; i < 3; i++)
    {
        snprintf (buf, 64, "SDL.Input.%d", i);
        config->getOption (buf, &device);

        if (device == "None")
        {
            UsrInputType[i] = SI_NONE;
        }
        else if (device.find ("GamePad") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_GAMEPAD : (int) SIFC_NONE;
        }
        else if (device.find ("PowerPad.0") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_POWERPADA : (int) SIFC_NONE;
        }
        else if (device.find ("PowerPad.1") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_POWERPADB : (int) SIFC_NONE;
        }
        else if (device.find ("QuizKing") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_QUIZKING;
        }
        else if (device.find ("HyperShot") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_HYPERSHOT;
        }
        else if (device.find ("Mahjong") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_MAHJONG;
        }
        else if (device.find ("TopRider") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_TOPRIDER;
        }
        else if (device.find ("FTrainer") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_FTRAINERA;
        }
        else if (device.find ("FamilyKeyBoard") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_FKB;
        }
        else if (device.find ("OekaKids") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_OEKAKIDS;
        }
        else if (device.find ("Arkanoid") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_ARKANOID : (int) SIFC_ARKANOID;
        }
        else if (device.find ("Shadow") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_SHADOW;
        }
        else if (device.find ("Zapper") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_ZAPPER : (int) SIFC_NONE;
        }
        else if (device.find ("BWorld") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_BWORLD;
        }
        else if (device.find ("4Player") != std::string::npos)
        {
            UsrInputType[i] = (i < 2) ? (int) SI_NONE : (int) SIFC_4PLAYER;
        }
        else
        {
            // Unknown device
            UsrInputType[i] = SI_NONE;
        }
    }

    // update each of the devices' configuration structure
    // XXX soules - this is temporary until this file is cleaned up to
    //              simplify the interface between configuration and
    //              structure data.  This will likely include the
    //              removal of multiple input buttons for a single
    //              input device key.
    int type, devnum, button;

    // gamepad 0 - 3
    for (unsigned int i = 0; i < GAMEPAD_NUM_DEVICES; i++)
    {
        char buf[64];
        snprintf (buf, 20, "SDL.Input.GamePad.%d.", i);
        prefix = buf;

        config->getOption (prefix + "DeviceType", &device);
        if (device.find ("Keyboard") != std::string::npos)
        {
            type = BUTTC_KEYBOARD;
        }
        else if (device.find ("Joystick") != std::string::npos)
        {
            type = BUTTC_JOYSTICK;
        }
        else
        {
            type = 0;
        }

        config->getOption (prefix + "DeviceNum", &devnum);
        for (unsigned int j = 0; j < GAMEPAD_NUM_BUTTONS; j++)
        {
            config->getOption (prefix + GamePadNames[j], &button);

            GamePadConfig[i][j].ButtType[0] = type;
            GamePadConfig[i][j].DeviceNum[0] = devnum;
            GamePadConfig[i][j].ButtonNum[0] = button;
            GamePadConfig[i][j].NumC = 1;
        }
    }

    // PowerPad 0 - 1
    for (unsigned int i = 0; i < POWERPAD_NUM_DEVICES; i++)
    {
        char buf[64];
        snprintf (buf, 20, "SDL.Input.PowerPad.%d.", i);
        prefix = buf;

        config->getOption (prefix + "DeviceType", &device);
        if (device.find ("Keyboard") != std::string::npos)
        {
            type = BUTTC_KEYBOARD;
        }
        else if (device.find ("Joystick") != std::string::npos)
        {
            type = BUTTC_JOYSTICK;
        }
        else
        {
            type = 0;
        }

        config->getOption (prefix + "DeviceNum", &devnum);
        for (unsigned int j = 0; j < POWERPAD_NUM_BUTTONS; j++)
        {
            config->getOption (prefix + PowerPadNames[j], &button);

            powerpadsc[i][j].ButtType[0] = type;
            powerpadsc[i][j].DeviceNum[0] = devnum;
            powerpadsc[i][j].ButtonNum[0] = button;
            powerpadsc[i][j].NumC = 1;
        }
    }

    // QuizKing
    prefix = "SDL.Input.QuizKing.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < QUIZKING_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + QuizKingNames[j], &button);

        QuizKingButtons[j].ButtType[0] = type;
        QuizKingButtons[j].DeviceNum[0] = devnum;
        QuizKingButtons[j].ButtonNum[0] = button;
        QuizKingButtons[j].NumC = 1;
    }

    // HyperShot
    prefix = "SDL.Input.HyperShot.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < HYPERSHOT_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + HyperShotNames[j], &button);

        HyperShotButtons[j].ButtType[0] = type;
        HyperShotButtons[j].DeviceNum[0] = devnum;
        HyperShotButtons[j].ButtonNum[0] = button;
        HyperShotButtons[j].NumC = 1;
    }

    // Mahjong
    prefix = "SDL.Input.Mahjong.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < MAHJONG_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + MahjongNames[j], &button);

        MahjongButtons[j].ButtType[0] = type;
        MahjongButtons[j].DeviceNum[0] = devnum;
        MahjongButtons[j].ButtonNum[0] = button;
        MahjongButtons[j].NumC = 1;
    }

    // TopRider
    prefix = "SDL.Input.TopRider.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < TOPRIDER_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + TopRiderNames[j], &button);

        TopRiderButtons[j].ButtType[0] = type;
        TopRiderButtons[j].DeviceNum[0] = devnum;
        TopRiderButtons[j].ButtonNum[0] = button;
        TopRiderButtons[j].NumC = 1;
    }

    // FTrainer
    prefix = "SDL.Input.FTrainer.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < FTRAINER_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + FTrainerNames[j], &button);

        FTrainerButtons[j].ButtType[0] = type;
        FTrainerButtons[j].DeviceNum[0] = devnum;
        FTrainerButtons[j].ButtonNum[0] = button;
        FTrainerButtons[j].NumC = 1;
    }

    // FamilyKeyBoard
    prefix = "SDL.Input.FamilyKeyBoard.";
    config->getOption (prefix + "DeviceType", &device);
    if (device.find ("Keyboard") != std::string::npos)
    {
        type = BUTTC_KEYBOARD;
    }
    else if (device.find ("Joystick") != std::string::npos)
    {
        type = BUTTC_JOYSTICK;
    }
    else
    {
        type = 0;
    }
    config->getOption (prefix + "DeviceNum", &devnum);
    for (unsigned int j = 0; j < FAMILYKEYBOARD_NUM_BUTTONS; j++)
    {
        config->getOption (prefix + FamilyKeyBoardNames[j], &button);

        fkbmap[j].ButtType[0] = type;
        fkbmap[j].DeviceNum[0] = devnum;
        fkbmap[j].ButtonNum[0] = button;
        fkbmap[j].NumC = 1;
    }
}
void ParseGIInput (FCEUGI * gi) {
    gametype = gi->type;

    CurInputType[0] = UsrInputType[0];
    CurInputType[1] = UsrInputType[1];
    CurInputType[2] = UsrInputType[2];

    if (gi->input[0] >= 0)
    {
        CurInputType[0] = gi->input[0];
    }
    if (gi->input[1] >= 0)
    {
        CurInputType[1] = gi->input[1];
    }
    if (gi->inputfc >= 0)
    {
        CurInputType[2] = gi->inputfc;
    }
    cspec = gi->cspecial;
}

void InitInputInterface ()
{
    void *InputDPtr;

    int t;
    int x;
    int attrib;

    for (t = 0, x = 0; x < 2; x++)
    {
        attrib = 0;
        InputDPtr = 0;

        switch (CurInputType[x])
        {
            case SI_POWERPADA:
            case SI_POWERPADB:
                InputDPtr = &powerpadbuf[x];
                break;
            case SI_GAMEPAD:
            case SI_SNES:
                InputDPtr = &JSreturn;
                break;
            case SI_ARKANOID:
                InputDPtr = MouseData;
                t |= 1;
                break;
            case SI_ZAPPER:
                InputDPtr = MouseData;
                t |= 1;
                attrib = 1;
                break;
            case SI_MOUSE:
            case SI_SNES_MOUSE:
                InputDPtr = MouseData;
                t |= 1;
                break;
        }
        FCEUI_SetInput (x, (ESI) CurInputType[x], InputDPtr, attrib);
    }

    attrib = 0;
    InputDPtr = 0;
    switch (CurInputType[2])
    {
        case SIFC_SHADOW:
            InputDPtr = MouseData;
            t |= 1;
            attrib = 1;
            break;
        case SIFC_OEKAKIDS:
            InputDPtr = MouseData;
            t |= 1;
            attrib = 1;
            break;
        case SIFC_ARKANOID:
            InputDPtr = MouseData;
            t |= 1;
            break;
        case SIFC_FKB:
            InputDPtr = fkbkeys;
            break;
        case SIFC_HYPERSHOT:
            InputDPtr = &HyperShotData;
            break;
        case SIFC_MAHJONG:
            InputDPtr = &MahjongData;
            break;
        case SIFC_QUIZKING:
            InputDPtr = &QuizKingData;
            break;
        case SIFC_TOPRIDER:
            InputDPtr = &TopRiderData;
            break;
        case SIFC_BWORLD:
            InputDPtr = BWorldData;
            break;
        case SIFC_FTRAINERA:
        case SIFC_FTRAINERB:
            InputDPtr = &FTrainerData;
            break;
    }

    FCEUI_SetInputFC ((ESIFC) CurInputType[2], InputDPtr, attrib);
    FCEUI_SetInputFourscore ((eoptions & EO_FOURSCORE) != 0);
}
