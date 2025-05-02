#include "m2s2/ecal/deserializer/deserializer_realsense.hpp"


int main(int argc, char** argv){
    std::string meas_path = "/home/m2s2/Nutramax_Data/test/Test8";    
    std::string channel_name = "rt/camera/color/image_raw";
    std::string out_path = "/home/m2s2/Nutramax_Data/test/Test8/realsense_pics/";
    //std::string meas_path =         argv[1];
    //std::string channel_name =      argv[2];
    //std::string out_path =          argv[4];

    m2s2::ecal::deserializer::DeserializerRealsense deserializer_realsense(meas_path, channel_name, out_path);

    deserializer_realsense.process_all();

    return 0;
}

namespace m2s2{ namespace ecal{ namespace deserializer{

    DeserializerRealsense::DeserializerRealsense(std::string meas_path, std::string channel_name, std::string out_path) :
        Deserializer(meas_path, channel_name),
        out_path(out_path)  
    {
        std::cout << "GOT REALSENSE IMAGES" << std::endl;

    }
    
    struct BaseMsg DeserializerRealsense::deserialize_message(
        eCAL::eh5::HDF5Meas* meas_,
        long long ID)
    {   

        struct Image* msg = &this->msg;
        
        size_t entry_size;
        if (!meas_->GetEntryDataSize(ID, entry_size)){
            std::cout << "Error Getting Entry Data Size" << std::endl;
            return this->msg;
        }
        
        uint8_t *data = new uint8_t[entry_size];
        if (!meas_->GetEntryData(ID, data)){
            std::cout << "Problem getting entry data: " << ID << std::endl;
            return this->msg;
        }

        // Copy this->msg context to an Image struct
        
        //Timestamp
        int ptr = 0;
        std::memcpy(&(this->msg.timestamp_sec), &data[ptr], sizeof((this->msg.timestamp_sec)));
        std::cout << "Timestamp secs: " <<  (this->msg.timestamp_sec) << std::endl;

        ptr += sizeof(this->msg.timestamp_sec);
        std::memcpy(&this->msg.timestamp_nanosec, &data[ptr], sizeof(this->msg.timestamp_nanosec));
        std::cout << "Timestamp nanosecs: " <<  this->msg.timestamp_nanosec << std::endl;

        ptr += sizeof(this->msg.timestamp_nanosec);

        // Frame ID
        uint64_t size_of_frameid;
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));

        ptr += sizeof(size_of_frameid);
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s((const char*)&frame_id_cstring, size_of_frameid);
        this->msg.ID = s;
        std::cout << "Frame ID: " <<  this->msg.ID << std::endl;

        ptr += size_of_frameid;
        std::memcpy(&(this->msg.height), &data[ptr], sizeof((this->msg.height)));
        std::cout << "Height: " <<  (this->msg.height) << std::endl;

        ptr += sizeof(this->msg.height);
        std::memcpy(&(this->msg.width), &data[ptr], sizeof((this->msg.width)));
        std::cout << "Width: " <<  (this->msg.width) << std::endl;

        ptr += sizeof(this->msg.width);

        // Encoding
        uint64_t size_of_encoding;
        std::memcpy(&size_of_encoding, &data[ptr], sizeof(size_of_encoding));
        
        ptr += sizeof(size_of_encoding);
        uint8_t *encoding_cstring = (uint8_t*)malloc(size_of_encoding);
        std::memcpy(&encoding_cstring, &data[ptr], sizeof(encoding_cstring));
        this->msg.encoding = std::string((const char*)&encoding_cstring);
        std::cout << "Encoding: " <<  this->msg.encoding << std::endl;

        ptr += size_of_encoding;
        std::memcpy(&(this->msg.is_bigendian), &data[ptr], sizeof((this->msg.is_bigendian)));
        std::cout << "Is Bigendian?: " <<  int(this->msg.is_bigendian) << std::endl;

        ptr += sizeof(this->msg.is_bigendian);
        std::memcpy(&(this->msg.step), &data[ptr], sizeof((this->msg.step)));
        std::cout << "Step: " <<  (this->msg.step) << std::endl;

        ptr += sizeof(this->msg.step);
        std::memcpy(&(this->msg.data_size), &data[ptr], sizeof((this->msg.data_size)));
        std::cout << "Data size: " <<  (this->msg.data_size) << std::endl;

        ptr += sizeof(this->msg.data_size);
        this->msg.data = (uint8_t*)malloc((size_t)this->msg.data_size);
        std::memcpy(&this->msg.data[0], &data[ptr], (size_t)(this->msg.data_size));    
        
        std::cout << "DONE DESERIALIZING" << std::endl;

        //delete [] data;

        return this->msg;
    }

    void DeserializerRealsense::process_message(struct BaseMsg* msg_){

        // Save raw image
        cv::Mat img_mat = cv::Mat(this->msg.height, this->msg.width, CV_8U, this->msg.data);
        std::string timestr = std::to_string(this->msg.timestamp_sec) + std::to_string(this->msg.timestamp_nanosec);
        std::string img_name = this->out_path + timestr + ".png";        
        cv::imwrite(img_name, img_mat);
        std::cout << "Image Saved" << std::endl;
        
        
    }

}}} // namespace m2s2, ecal, deserializer