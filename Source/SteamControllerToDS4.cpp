#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <thread>
#include "steam_controller/steam_controller.hpp"
#include "ViGEm/Client.h"
#pragma comment(lib, "setupapi.lib")

DS4_OUTPUT_DATA output;
bool lookingForOutput = true;
bool leave = false;
bool outputFound = false;
DS4_TOUCH prevTouchBuffer1 = { 0 };
DS4_TOUCH prevTouchBuffer2 = { 0 };


void ds4Output(const PVIGEM_CLIENT client, const PVIGEM_TARGET pad) {
    while (lookingForOutput) {
        vigem_target_ds4_get_output(client, pad, &output);
        outputFound = true;
    }
}

void ds4Update(steam_controller::update_event& update, const PVIGEM_CLIENT client, const PVIGEM_TARGET pad, DS4_REPORT_EX& report) {
    
    RtlZeroMemory(&report, sizeof(DS4_REPORT_EX));
    report.Report.bTouchPacketsN = 3;
    report.Report.sCurrentTouch = { 0 };
    report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
    report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x80;
    report.Report.sCurrentTouch.bIsUpTrackingNum2 = 0x80;
    if (update.buttons & static_cast<int>(steam_controller::Button::RT)) report.Report.wButtons |= DS4_BUTTON_TRIGGER_RIGHT;
    if (update.buttons & static_cast<int>(steam_controller::Button::LT)) report.Report.wButtons |= DS4_BUTTON_TRIGGER_LEFT;
    if (update.buttons & static_cast<int>(steam_controller::Button::RS)) report.Report.wButtons |= DS4_BUTTON_SHOULDER_RIGHT;
    if (update.buttons & static_cast<int>(steam_controller::Button::LS)) report.Report.wButtons |= DS4_BUTTON_SHOULDER_LEFT;
    if (update.buttons & static_cast<int>(steam_controller::Button::Y)) report.Report.wButtons |= DS4_BUTTON_TRIANGLE;
    if (update.buttons & static_cast<int>(steam_controller::Button::B)) report.Report.wButtons |= DS4_BUTTON_CIRCLE;
    if (update.buttons & static_cast<int>(steam_controller::Button::X)) report.Report.wButtons |= DS4_BUTTON_SQUARE;
    if (update.buttons & static_cast<int>(steam_controller::Button::A)) report.Report.wButtons |= DS4_BUTTON_CROSS;
    if (update.buttons & static_cast<int>(steam_controller::Button::PREV)) report.Report.wButtons |= DS4_BUTTON_SHARE;
    if (update.buttons & static_cast<int>(steam_controller::Button::HOME)) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_PS;
    if (update.buttons & static_cast<int>(steam_controller::Button::NEXT)) report.Report.wButtons |= DS4_BUTTON_OPTIONS;
    report.Report.bTriggerL = update.left_trigger;
    report.Report.bTriggerR = update.right_trigger;
    if (!(update.buttons & static_cast<int>(steam_controller::Button::RG))) {
        if ((update.right_axis.x) / 128 >= 128) {
            report.Report.bThumbRX = 255;
        }
        else if ((update.right_axis.x) / 128 <= -128) {
            report.Report.bThumbRX = 0;
        }
        else {
            report.Report.bThumbRX = (update.right_axis.x) / 128 + 128;
        }
        if ((update.right_axis.y) / 128 >= 128) {
            report.Report.bThumbRY = 0;
        }
        else if ((update.right_axis.y) / 128 <= -128) {
            report.Report.bThumbRY = 255;
        }
        else {
            report.Report.bThumbRY = -((update.right_axis.y) / 128) + 128;
        }
        /*
        report.Report.bThumbRX = ((update.right_axis.x) / 256 + 127);
        report.Report.bThumbRY = (-(update.right_axis.y) / 256 + 127);
        */
        // Steam stores touchpad input on a signed 32bit integer, which must be converted to an unsigned byte. 
        // Steam and PS4 y-axes are flipped for some reason.
        if (update.buttons & static_cast<int>(steam_controller::Button::RPAD)) report.Report.wButtons |= DS4_BUTTON_THUMB_RIGHT;
    }
    if ((update.buttons & static_cast<int>(steam_controller::Button::RG)) && !(update.buttons & static_cast<int>(steam_controller::Button::LG))) {
        report.Report.bThumbRX = 128;
        report.Report.bThumbRY = 128;
        if (update.buttons & static_cast<int>(steam_controller::Button::RPAD)) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
        if (update.buttons & static_cast<int>(steam_controller::Button::RFINGER)) {
            report.Report.bTouchPacketsN = 1;
            report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
            report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x00;
            report.Report.sCurrentTouch.bTouchData1[0] = (((update.right_axis.x) / 35) + 960) & 0xFF;
            report.Report.sCurrentTouch.bTouchData1[1] = (((((update.right_axis.x) / 35) + 960) >> 8) & 0x0F) | (((((-update.right_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData1[2] = ((((-update.right_axis.y) / 70) + 472) >> 4) & 0xFF;
        }
    }
    if ((update.buttons & static_cast<int>(steam_controller::Button::LG)) && !(update.buttons & static_cast<int>(steam_controller::Button::RG))) {
        report.Report.wButtons |= DS4_BUTTON_DPAD_NONE;
        if (update.buttons & static_cast<int>(steam_controller::Button::LFINGER)) {
            if (update.buttons & static_cast<int>(steam_controller::Button::STICK)) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
            report.Report.bTouchPacketsN = 1;
            report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
            report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x00;
            report.Report.sCurrentTouch.bTouchData1[0] = (((update.left_axis.x) / 35) + 960) & 0xFF;
            report.Report.sCurrentTouch.bTouchData1[1] = (((((update.left_axis.x) / 35) + 960) >> 8) & 0x0F) | (((((-update.left_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData1[2] = ((((-update.left_axis.y) / 70) + 472) >> 4) & 0xFF;
        }
    }
    if ((update.buttons & static_cast<int>(steam_controller::Button::RG)) && (update.buttons & static_cast<int>(steam_controller::Button::LG))) {
        report.Report.bThumbRX = 128;
        report.Report.bThumbRY = 128;
        report.Report.wButtons |= DS4_BUTTON_DPAD_NONE;
        if ((update.buttons & static_cast<int>(steam_controller::Button::LFINGER)) && !(update.buttons & static_cast<int>(steam_controller::Button::RFINGER))) {
            if (update.buttons & static_cast<int>(steam_controller::Button::STICK)) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
            report.Report.bTouchPacketsN = 1;
            report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
            report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x00;
            report.Report.sCurrentTouch.bTouchData1[0] = (((update.left_axis.x) / 69) + 480) & 0xFF;
            report.Report.sCurrentTouch.bTouchData1[1] = (((((update.left_axis.x) / 69) + 480) >> 8) & 0x0F) | (((((-update.left_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData1[2] = ((((-update.left_axis.y) / 70) + 472) >> 4) & 0xFF;
        }
        if ((update.buttons & static_cast<int>(steam_controller::Button::RFINGER)) && !(update.buttons & static_cast<int>(steam_controller::Button::LFINGER))) {
            if (update.buttons & static_cast<int>(steam_controller::Button::RPAD)) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
            report.Report.bTouchPacketsN = 1;
            report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
            report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x00;
            report.Report.sCurrentTouch.bTouchData1[0] = (((update.right_axis.x) / 69) + 1440) & 0xFF;
            report.Report.sCurrentTouch.bTouchData1[1] = (((((update.right_axis.x) / 69) + 1440) >> 8) & 0x0F) | (((((-update.right_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData1[2] = ((((-update.right_axis.y) / 70) + 472) >> 4) & 0xFF;
        }
        if ((update.buttons & static_cast<int>(steam_controller::Button::RFINGER)) && (update.buttons & static_cast<int>(steam_controller::Button::LFINGER))) {
            if ((update.buttons & static_cast<int>(steam_controller::Button::STICK)) || (update.buttons & static_cast<int>(steam_controller::Button::RPAD))) report.Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
            report.Report.bTouchPacketsN = 2;
            report.Report.sCurrentTouch.bPacketCounter = update.time_stamp;
            report.Report.sCurrentTouch.bIsUpTrackingNum1 = 0x00;
            report.Report.sCurrentTouch.bTouchData1[0] = (((update.left_axis.x) / 69) + 480) & 0xFF;
            report.Report.sCurrentTouch.bTouchData1[1] = (((((update.left_axis.x) / 69) + 480) >> 8) & 0x0F) | (((((-update.left_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData1[2] = ((((-update.left_axis.y) / 70) + 472) >> 4) & 0xFF;
            report.Report.sCurrentTouch.bIsUpTrackingNum2 = 0x00;
            report.Report.sCurrentTouch.bTouchData2[0] = (((update.right_axis.x) / 69) + 1440) & 0xFF;
            report.Report.sCurrentTouch.bTouchData2[1] = (((((update.right_axis.x) / 69) + 1440) >> 8) & 0x0F) | (((((-update.right_axis.y) / 70) + 472) & 0x0F) << 4);
            report.Report.sCurrentTouch.bTouchData2[2] = ((((-update.right_axis.y) / 70) + 472) >> 4) & 0xFF;

        }
    }
    if (!(update.buttons & static_cast<int>(steam_controller::Button::LG))) {
        if (update.buttons & static_cast<int>(steam_controller::Button::DPAD_UP)) report.Report.wButtons |= DS4_BUTTON_DPAD_NORTH;
        else if (update.buttons & static_cast<int>(steam_controller::Button::DPAD_RIGHT)) report.Report.wButtons |= DS4_BUTTON_DPAD_EAST;
        else if (update.buttons & static_cast<int>(steam_controller::Button::DPAD_LEFT)) report.Report.wButtons |= DS4_BUTTON_DPAD_WEST;
        else if (update.buttons & static_cast<int>(steam_controller::Button::DPAD_DOWN)) report.Report.wButtons |= DS4_BUTTON_DPAD_SOUTH;
        else report.Report.wButtons |= DS4_BUTTON_DPAD_NONE;
    }
    if (!(update.buttons & static_cast<int>(steam_controller::Button::LFINGER))) {
        report.Report.bThumbLX = update.left_axis.x / 256 + 127;
        report.Report.bThumbLY = -(update.left_axis.y) / 256 + 127;
        if (update.buttons & static_cast<int>(steam_controller::Button::STICK)) report.Report.wButtons |= DS4_BUTTON_THUMB_LEFT;
    }
    else {
        report.Report.bThumbLX = 128;
        report.Report.bThumbLY = 128;
    }
    report.Report.wTimestamp = update.time_stamp;
    report.Report.bBatteryLvl = 255;
    report.Report.wGyroX = update.angular_velocity.x;
    report.Report.wGyroY = update.angular_velocity.z; // Y and Z axes for gyro and acceleration are flipped from Steam to PS4 for some reason.
    report.Report.wGyroZ = -(update.angular_velocity.y);
    report.Report.wAccelX = update.acceleration.x;
    report.Report.wAccelY = update.acceleration.z;
    report.Report.wAccelZ = -(update.acceleration.y);
    report.Report.sPreviousTouch[0] = prevTouchBuffer1;
    report.Report.sPreviousTouch[1] = prevTouchBuffer2;
    vigem_target_ds4_update_ex(client, pad, report);
    prevTouchBuffer2 = prevTouchBuffer1;
    prevTouchBuffer1 = report.Report.sCurrentTouch;
    return;
}

void tryOpening(steam_controller::context& context, steam_controller::connection_info const& info, const PVIGEM_CLIENT& client, const PVIGEM_TARGET& pad, DS4_REPORT_EX& report)
{

    auto controller = context.connect(info, 0, std::chrono::milliseconds(500));
    if (!controller)
        return;

    system("cls");
    std::cout << "Press ALT + ESC to exit the program.\n";
    std::cout << "Controller connected\n";

    steam_controller::event event {};

    while (true)
    {
        if ((GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
            leave = true;
            break;
        }

        controller->poll(event);

        auto state = controller->state();
        if (state == steam_controller::connection_state::disconnected)
        {
            // Note: controllers that are never attached will still appear as disconnected.
            // Need to keep searching for one that is connected!
            controller = context.connect(info, 0, std::chrono::milliseconds(500));
            if (!controller) {
                system("cls");
                std::cout << "Press ALT + ESC to exit the program.\n";
                std::cout << "Controller disconnected\n";
                break;
            }
        }


        if (event.key == steam_controller::event_key::UPDATE)
        {
            ds4Update(event.update, client, pad, report);
            if (outputFound) {
                //std::cout << "\nLarge: " << static_cast<uint16_t>(output.LargeMotor) << ", Small: " << static_cast<uint16_t>(output.SmallMotor);
                try {
                    controller->feedback(steam_controller::hand::left, output.LargeMotor * 256, std::chrono::microseconds(10), 5);
                }
                catch (...) {
                    controller->feedback(steam_controller::hand::left, 65535, std::chrono::microseconds(10), 2);
                }
                try {
                    controller->feedback(steam_controller::hand::right, output.SmallMotor * 64, std::chrono::microseconds(10), 5);
                }
                catch (...) {
                    controller->feedback(steam_controller::hand::right, 16320, std::chrono::microseconds(10), 2);
                }
                outputFound = false;
            }
            //Sleep(1);
        }
    }
}



int main()
{
    //SetConsoleTitle("SteamControllerToDS4 1.0.0");

    const auto client = vigem_alloc();

    if (client == nullptr)
    {
        std::cerr << "Uh, not enough memory to do that?!" << std::endl;
        return -1;
    }

    const auto retval = vigem_connect(client);

    if (!VIGEM_SUCCESS(retval))
    {
        std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
        return -1;
    }

    // Allocate handle to identify new pad
    const auto pad = vigem_target_ds4_alloc();
    // Add client to the bus, this equals a plug-in event
    auto pir = vigem_target_add(client, pad);
    // Error handling
    if (!VIGEM_SUCCESS(pir))
    {
        std::cerr << "Target plugin failed with error code: 0x" << std::hex << pir << std::endl;
        return -1;
    }

    DS4_REPORT_EX report;
    
    std::thread outputThread(ds4Output, client, pad);
    steam_controller::context context;
    auto all = context.enumerate();
    std::cout << "Press ALT + ESC to exit the program.\n";
    if (all.empty())
    {
        std::cout << "No Steam Controllers detected! Please make a wired connection via USB.";
        while (all.empty())
        {
            if ((GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                lookingForOutput = false;
                vigem_disconnect(client);
                vigem_free(client);
                outputThread.join();
                return 0;
            }
            all = context.enumerate();
            Sleep(1);
        }
    }
    system("cls");
    std::cout << "Press ALT + ESC to exit the program.\n";
    while (!((GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_ESCAPE) & 0x8000))) {
        for (auto const& each : all) {
            if (leave == true) {
                break;
            }
            tryOpening(context, each, client, pad, report);
        }
        all = context.enumerate();
    }
    system("cls");
    std::cout << "Exiting...";
    lookingForOutput = false;
    vigem_disconnect(client);
    vigem_free(client);
    outputThread.join();
    return 0;
}