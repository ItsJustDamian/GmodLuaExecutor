#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "../imgui/imgui.h"

namespace Notifications
{
	std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> notifications;

	void AddNotification(const std::string& notification)
	{
		auto now = std::chrono::high_resolution_clock::now();
		notifications.push_back({ notification, now });
	}

	void RenderNotifications()
	{
		// Remove first added if size is above 5
		if (notifications.size() > 5)
		{
			notifications.erase(notifications.begin());
		}

		int index = 0;
		for (auto it = notifications.begin(); it != notifications.end();)
		{
			auto& [message, timestamp] = *it;
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - timestamp).count();
			if (elapsed > 5) // Remove notification after 5 seconds
			{
				it = notifications.erase(it);
			}
			else
			{
				// Draw it on the screen in a little box using drawlist
				ImVec2 textSize = ImGui::CalcTextSize(it->first.c_str());
				float x0 = 5.0f; // left margin
				float y0 = 5.0f + index * (25 + 5); // same as before
				float padding = 10.0f; // horizontal padding on each side

				// Rectangle from (x0, y0) to (x0 + text width + padding * 2, y0 + boxHeight)
				ImGui::GetBackgroundDrawList()->AddRectFilled(
					ImVec2(x0, y0),
					ImVec2(x0 + textSize.x + padding * 2, y0 + 25),
					IM_COL32(79, 8, 112, 200)
				);

				// Draw the text inside with left padding
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(x0 + padding, y0 + (25 - textSize.y) * 0.5f), // vertically center
					IM_COL32(255, 255, 255, 255),
					it->first.c_str()
				);
				++it;
				++index;
			}
		}
	}
}