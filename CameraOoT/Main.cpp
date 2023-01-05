#include <Windows.h>
#include <SDL.h>
#include <iostream>
#include "Proc.h"
#include "Time.h"

#define PI 3.14159265359f
#define DEAD_ZONE_STICK 0.20f // Max 1.0f
#define X_ANGLE_SPEED 150.0f  // Degree Second
#define Y_ANGLE_SPEED 200.0f  // ~Degree Second

using namespace std;

int main(int, char**)
{
	SetConsoleTitle(L"OoT Camera");

	const DWORD proc_id = GetProcId(L"citra-qt.exe");
	if (proc_id == 0)
	{
		std::cout << "citra-qt.exe not found" << std::endl;
		system("pause");
		return 0;
	}
	std::cout << "citra-qt.exe found" << std::endl;

	const HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, proc_id);
	if (h_process == INVALID_HANDLE_VALUE)
	{
		std::cout << "Process invalid" << std::endl;
		system("pause");
		return 1;
	}
	std::cout << "Successfully hooked to citra-qt.exe, finding memory addresses..." << std::endl;

	const uint8_t pb_pattern_pp[17] = {
		0x2A, 0x00, 0x00, 0x60, 0x6A, 0x8F, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xFF, 0x75, 0x00 };
	const uint8_t pb_pattern_pc[17] = {
		0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01 };
	const auto mask = "xxxxxxxxxxxxxxxx";
	const uintptr_t local_player = SearchInProcessMemory(h_process, pb_pattern_pp, mask) + 0x33;
	const uintptr_t link_crawl = local_player - 0x33 + 0x250B;
	const uintptr_t link_on_epona = local_player - 0x33 + 0x131;
	printf("[+] Found LocalPlayer @ 0x%X\n", static_cast<unsigned>(local_player));
	const uintptr_t local_camera = SearchInProcessMemory(h_process, pb_pattern_pc, mask) + 0xB6;
	const uintptr_t look_at_camera = local_camera - 0xB6 + 0x60;
	printf("[+] Found LocalCamera @ 0x%X\n", static_cast<unsigned>(local_camera));

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GameController* controller = nullptr;
	SDL_Event event;

	Time time;
	float base_angle = 0.0f;
	float base_height = 50.0f;
	time.StartTime();
	float dx, dz, dy;
	constexpr uint16_t look_link = 60;
	uint16_t sneek_link = 11012;
	uint16_t epona_link = 0;

	float joystick_x = 0.0f;
	float joystick_y = 0.0f;
	bool resetangle = true;
	bool pause = true;

	while (true)
	{
		time.FixedUpdateTime();

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_CONTROLLERDEVICEADDED:
				controller = SDL_GameControllerOpen(event.cdevice.which);
				if (controller)
				{
					std::cout << "[+] Controller " << event.cdevice.which << " is connected" << std::endl;
				}
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				SDL_GameControllerClose(controller);
				std::cout << "[X] Controller " << event.cdevice.which << " is not connected" << std::endl;
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
				{
					resetangle = true;
					pause = true;
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
				{
					joystick_x = event.caxis.value / 32767.0f;
					if (joystick_x < DEAD_ZONE_STICK && joystick_x > -DEAD_ZONE_STICK)
					{
						joystick_x = 0.0f;
					}
					else
					{
						pause = false;
					}
				}
				else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
				{
					joystick_y = event.caxis.value / 32767.0f;
					if (joystick_y < DEAD_ZONE_STICK && joystick_y > -DEAD_ZONE_STICK)
					{
						joystick_y = 0.0f;
					}
					else
					{
						pause = false;
					}
				}
				break;
			default:
				break;
			}
		}

		ReadProcessMemory(h_process, reinterpret_cast<void*>(link_crawl), &sneek_link, sizeof(uint16_t), nullptr);
		ReadProcessMemory(h_process, reinterpret_cast<void*>(link_on_epona), &epona_link, sizeof(uint16_t), nullptr);

		if (!pause && sneek_link != 21012 && epona_link != 2448)
		{
			constexpr float lenght_base = 250.0f;
			WriteProcessMemory(h_process, reinterpret_cast<void*>(look_at_camera), &look_link, sizeof(uint16_t), nullptr);
			ReadProcessMemory(h_process, reinterpret_cast<void*>(local_player), &x, sizeof(float), nullptr);
			ReadProcessMemory(h_process, reinterpret_cast<void*>(local_player + 0x04), &y, sizeof(float), nullptr);
			ReadProcessMemory(h_process, reinterpret_cast<void*>(local_player + 0x08), &z, sizeof(float), nullptr);

			if (resetangle)
			{
				resetangle = false;
				ReadProcessMemory(h_process, reinterpret_cast<void*>(local_camera), &dx, sizeof(float), nullptr);
				ReadProcessMemory(h_process, reinterpret_cast<void*>(local_camera + 0x04), &dy, sizeof(float), nullptr);
				ReadProcessMemory(h_process, reinterpret_cast<void*>(local_camera + 0x08), &dz, sizeof(float), nullptr);
				base_angle = atan2(x - dx, z - dz) * -180.0f / PI;
				base_angle -= 90.0f;
				if (base_angle < -180.0f)
				{
					base_angle += 360.0f;
				}
				else if (base_angle > 180.0f)
				{
					base_angle -= 360.0f;
				}
			}

			base_angle += time.GetFixedDeltaTime() * joystick_x * X_ANGLE_SPEED;
			if (base_angle > 180.0f)
			{
				base_angle = -179.999999f;
			}
			else if (base_angle < -180.0f)
			{
				base_angle = 179.9999999f;
			}

			base_height -= time.GetFixedDeltaTime() * joystick_y * Y_ANGLE_SPEED;
			if (base_height > 175.0f)
			{
				base_height = 175.0f;
			}
			else if (base_height < 0.0f)
			{
				base_height = 0.0f;
			}

			const float theta = base_angle * PI / 180.0f;
			dx = (cos(theta) * lenght_base) + x;
			dy = base_height + y;
			dz = (sin(theta) * lenght_base) + z;

			if (!resetangle)
			{
				WriteProcessMemory(h_process, reinterpret_cast<void*>(local_camera), &dx, sizeof(float), nullptr);
				WriteProcessMemory(h_process, reinterpret_cast<void*>(local_camera + 0x04), &dy, sizeof(float), nullptr);
				WriteProcessMemory(h_process, reinterpret_cast<void*>(local_camera + 0x08), &dz, sizeof(float), nullptr);
			}
		}
	}
}
