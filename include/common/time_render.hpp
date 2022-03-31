#include <sstream>

class TimeRender
{
    public:
        static TimeRender& getInstance()
        {
            static TimeRender    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            return instance;
        }

    private:
        TimeRender() {} 

    public:
        TimeRender(TimeRender const&) = delete;
        void operator=(TimeRender const&) = delete;
        static std::string HumanReadableTime(sf::Int32 time) { // in ms
            const int hours = time / 3600000;
            time -= hours * 3600000;
            const int minutes = time / 60000;
            time -= minutes * 60000;
            const int seconds = time / 1000;
            time -= seconds * 1000;
            const int millis = time;

            
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
            return ss.str();
        }
};