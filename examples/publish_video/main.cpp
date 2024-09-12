#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono>
#include <thread>
#include <numeric>

#include <opencv4/opencv2/opencv.hpp>

#include <emessgee.h>

using namespace std::chrono_literals;
typedef char byte;

inline double time()
{
    auto currentTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(currentTime.time_since_epoch());

    return duration.count();
}

void publish_thread(std::string topic, std::filesystem::path video_path, bool* running)
{
    cv::VideoCapture video(video_path);
    size_t num_frames = video.get(cv::CAP_PROP_FRAME_COUNT);
    size_t width = video.get(cv::CAP_PROP_FRAME_WIDTH);
    size_t height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
    size_t image_size_in_bytes = width * height * 3;
    size_t queue_size = 10;
    emessgee::Publisher publisher(topic, image_size_in_bytes * queue_size, queue_size);

    std::vector<double> dts;
    double start = time();

    for(int i = 0; i < num_frames; i++)
    {
        cv::Mat image;
        bool valid = video.read(image);
        emessgee::byte* image_ptr = image.ptr<emessgee::byte>();

        publisher.send(topic, image_ptr, image_size_in_bytes);

        double current_time = time();
        double dt = current_time - start;
        start = current_time;

        dts.push_back(dt);
    }

    video.release(); 
    publisher.close();

    *running = false;

    double avg_sub_dt = std::accumulate(dts.begin(), dts.end(), 0.0) / dts.size();
    avg_sub_dt *= 1000.0f;
    std::cout << "AVG pub dt: " << avg_sub_dt << "ms" << std::endl;
}

int main(int argc, char *argv[])
{
    bool running = true;
    std::string topic = "video_topic";
    std::filesystem::path video_path = "";
    emessgee::Subscriber sub(topic);

    if(argc == 2)
    {
        video_path = argv[1];
    }

    if(!std::filesystem::exists(video_path))
    {
        std::cout << video_path << " does not exist" << std::endl;
        return 1;
    }
    cv::VideoCapture video(video_path);
    size_t width = video.get(cv::CAP_PROP_FRAME_WIDTH);
    size_t height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
    video.release();

    std::thread pub_thread = std::thread(publish_thread, topic, video_path, &running);

    std::vector<double> dts;
    double start = time();

    while(running)
    {
        emessgee::ReadResult result = sub.recv(topic);

        if(result.valid)
        {
            cv::Mat image(height, width, CV_8UC3, result.data);

            double current_time = time();
            double dt = current_time - start;
            start = current_time;

            dts.push_back(dt);

            cv::imshow("image", image);
            int key = cv::waitKey(1);

            if(key != -1)
            {
                running = false;
                break;
            }
        }
    }

    pub_thread.join();

    double avg_sub_dt = std::accumulate(dts.begin(), dts.end(), 0.0) / dts.size();
    avg_sub_dt *= 1000.0f;

    std::cout << "AVG sub dt: " << avg_sub_dt << "ms" << std::endl;

    return 0;
}
