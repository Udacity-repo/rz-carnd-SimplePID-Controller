#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  // Initialize the PID instances and its variables.

  PID pid;
  double kp = 0.06; // best fit based on trial and error (records presented in comments below) 
  double ki = 0.00; // No need to compensate for systematic bias (we don't have them in the simulator). 
  double kd = 0.50; // best fit based on trial and error (records presented in comments below)
  pid.Init(kp, ki, kd);

  /*
	0.05 0.0 0.0 - Initial guess, looks like proportion compensation (P) is not agressive enough to make curves.
	0.1 0.0 0.0  - P controller increased, oscillation looks fast enough but it is badly overshooting, it needs to adjust D controller.
	0.1 0.0 0.5  - D controller increased, it drives very well for Throttle = 0.3. Let's try higher speeds.
	0.1 0.0 0.5  - Tried the same hyperparameters for Throttle = 0.5, but noted that faster we go, less P controller we need, let's play with that.
	0.06 0.0 0.5 - As expected, decreasing P for faster speed works. It drives ok for Throttle = 0.5.
				   The funny thing here is that in my computer I was running the simulator using window 800x600 in "simple" mode,
				   and it was doing ok, but when I turned on the software to record my screen it started to fail. Then I turned off the software to record
				   and the model was back to normal and doing ok. So my guess is that the software recording was making the steering calculation slower.
				   I decided to improve the runtime by using the window 800x600 in "fast" mode, and then I could make it fine even with the software
				   to record the screen turned on. Interesting finding.
	0.03 0.0 0.5 - I did several trials for Throttle = 0.7 by reducing P controller, but for this track my car had a high average speed for Throttle=0.5 already.
				   My bet is that I may increase speed in straight sections of the track, but it would require to PID control the speed as well.
				   I will not implement it at this time, and will revisit the project in the future for futher improvements.
  */

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          //double speed = std::stod(j[1]["speed"].get<std::string>());
          //double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          
          pid.UpdateError(cte);
          steer_value = -pid.TotalError();
          
          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.5;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
