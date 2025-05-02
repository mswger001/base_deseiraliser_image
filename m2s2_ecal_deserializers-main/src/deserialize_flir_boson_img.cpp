#include "m2s2/ecal/deserializer/deserializer_boson_img.hpp"


int main(int argc, char** argv){
    //std::string meas_path = "/home/m2s2/Nutramax_Data/test/Test11";
    //std::string channel_name = "rt/boson/image_raw";
    //std::string out_path = "/home/m2s2/m2s2_ws/m2s2_ecal_deserializers/test_boson/";

    std::string meas_path =         argv[1];
    std::string channel_name =      argv[2];
    std::string out_path =          argv[3];

    m2s2::ecal::deserializer::DeserializerBosonImg flir_boson_deserializer_img(meas_path, channel_name, out_path);

    flir_boson_deserializer_img.process_all();

    return 0;
}

namespace m2s2{ namespace ecal{ namespace deserializer{

    DeserializerBosonImg::DeserializerBosonImg(std::string meas_path, std::string channel_name, std::string out_path) :
        Deserializer(meas_path, channel_name),
        out_path(out_path)  
    {
        std::cout << "GOT BOSON IMAGES" << std::endl;

    }
    
    struct BaseMsg DeserializerBosonImg::deserialize_message(
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

        delete [] data;

        return this->msg;
    }

    void DeserializerBosonImg::process_message(struct BaseMsg* msg_){

        // Save raw image
        cv::Mat img_mat = cv::Mat(this->msg.height, this->msg.width, CV_8UC1, this->msg.data);
        std::string timestr = std::to_string(this->msg.timestamp_sec) + "_" + std::to_string(this->msg.timestamp_nanosec);
        std::string img_name = this->out_path + timestr + ".png";        
        cv::imwrite(img_name, ~img_mat);
        std::cout << "Image Saved" << std::endl;
    }

}}} // namespace m2s2, ecal, deserializer