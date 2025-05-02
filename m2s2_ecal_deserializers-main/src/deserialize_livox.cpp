#include "m2s2/ecal/deserializer/deserializer_livox.hpp"

int main(int argc, char** argv){

    std::string meas_path = "/home/m2s2/ecal_meas/lidar_rs_7";
    std::string channel_name = "rt/livox/lidar";
    std::string out_path = "/home/m2s2/ecal_meas/lidar7/";

    //std::string meas_path =         argv[1];
    //std::string channel_name =      argv[2];
    //std::string out_path =          argv[3];

    m2s2::ecal::deserializer::DeserializerLivox livox_deserializer(meas_path, channel_name, out_path);

    livox_deserializer.process_all();

    return 0;
}

namespace m2s2{ namespace ecal{ namespace deserializer{

    DeserializerLivox::DeserializerLivox(std::string meas_path, std::string channel_name, std::string out_path) :
        Deserializer(meas_path, channel_name),
        out_path(out_path)
  
    {   
    }
    
    struct BaseMsg DeserializerLivox::deserialize_message(
        eCAL::eh5::HDF5Meas* meas_,
        long long ID)
    {   

        struct PointCloud2* msg = &this->msg;
        
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

        // Copy this->msg context to a PointCloud and PointField structs
        
        // PCL STUFF: UNCOMMENT NEXT LINE TO SAVE TO PCD - create pcl pointcloud2 
        pcl::PCLPointCloud2* pcl_pc2 = &this->pcl_pc2;

        int ptr = 0;
        std::memcpy(&this->msg.timestamp_sec, &data[ptr], sizeof(this->msg.timestamp_sec));
        //std::cout << "Timestamp secs: " <<  this->msg.timestamp_sec << std::endl;

        ptr += sizeof(this->msg.timestamp_sec);
        std::memcpy(&this->msg.timestamp_nanosec, &data[ptr], sizeof(this->msg.timestamp_nanosec));
        //std::cout << "Timestamp nanosecs: " <<  this->msg.timestamp_nanosec << std::endl;

        ptr += sizeof(this->msg.timestamp_nanosec);

        uint64_t size_of_frameid;
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));
        
        ptr += sizeof(size_of_frameid);
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s = (const char*)&frame_id_cstring;
        this->msg.ID = s;
        //std::cout << "Frame ID: " <<  this->msg.ID << std::endl;

        
        // PCL STUFF: UNCOMMENT FOLLOWING LINES TO SAVE TO PCD - create pcl header
        pcl::PCLHeader* pcl_header = &this->pcl_header;
        this->pcl_header.stamp = static_cast<std::uint64_t>(this->msg.timestamp_nanosec) / 1000ull;
        this->pcl_header.seq = 0;
        this->pcl_header.frame_id = this->msg.ID;
        this->pcl_pc2.header = this->pcl_header;

        ptr += size_of_frameid;
        std::memcpy(&this->msg.height, &data[ptr], sizeof(this->msg.height));
        //std::cout << "Height: " <<  this->msg.height << std::endl;

        ptr += sizeof(this->msg.height);
        std::memcpy(&this->msg.width, &data[ptr], sizeof(this->msg.width));
        //std::cout << "Width: " <<  this->msg.width << std::endl;
        
        ptr += sizeof(this->msg.width);
        
        long pf_arr_size;
        std::memcpy(&pf_arr_size, &data[ptr], sizeof(pf_arr_size));
        ptr += sizeof(pf_arr_size);
        uint8_t *pointField_tmp = (uint8_t*)malloc((size_t)(121));
        std::memcpy(&pointField_tmp[0], &data[ptr], (size_t)(121));   
        
        //PCL STUFF: UNCOMMENT FOLLOWING 2 LINES TO SAVE TO PCD - populate pcl metadata
        this->pcl_pc2.height = this->msg.height;
        this->pcl_pc2.width = this->msg.width;

        //PointField array
        int pf_ptr = 0;
        //std::cout << "Fields: " << std::endl;

        // PCL STUFF: UNCOMMENT NEXT LINE TO SAVE TO PCD - create pcl pointfield
        std::vector<pcl::PCLPointField>* pcl_pfs = &this->pcl_pfs;

        for (int i=0; i<6; i++){
            pcl::PCLPointField* pcl_pf = &this->pcl_pf; //PCL STUFF: UNCOMMENT TO SAVE TO PCD
            struct PointField* pf_msg = &this->pf_msg;

            uint64_t name_size;
            std::memcpy(&name_size, &pointField_tmp[pf_ptr], sizeof(name_size));

            pf_ptr += sizeof(name_size);
            uint8_t *name_string = (uint8_t*)malloc((size_t)name_size);
            std::memcpy(&name_string, &pointField_tmp[pf_ptr], sizeof(name_string));
            std::string n = (const char*)&name_string;
            this->pf_msg.name = n;

            if (i==1){
                this->pf_msg.name = "y";
            }
            else if (i==2){
                this->pf_msg.name = "z";
            }
            else if (i==3){
                this->pf_msg.name = "intensity";
            }

            //std::cout << "- Name: " <<  this->pf_msg.name << std::endl;

            pf_ptr += name_size;
            std::memcpy(&this->pf_msg.offset, &pointField_tmp[pf_ptr], sizeof(this->pf_msg.offset));
            //std::cout << "  Offset: " <<  this->pf_msg.offset << std::endl;
 
            pf_ptr += sizeof(this->pf_msg.offset);
            std::memcpy(&this->pf_msg.datatype, &pointField_tmp[pf_ptr], sizeof(this->pf_msg.datatype));
            //std::cout << "  Datatype: " <<  int(this->pf_msg.datatype) << std::endl;

            pf_ptr += sizeof(this->pf_msg.datatype);
            std::memcpy(&this->pf_msg.count, &pointField_tmp[pf_ptr], sizeof(this->pf_msg.count));
            //std::cout << "  Count: " <<  this->pf_msg.count << std::endl;
            
            pf_ptr += sizeof(this->pf_msg.count);

            // PCL STUFF: UNCOMMENT FOLLOWING LINES TO SAVE TO PCD - populate pointfield data
            this->pcl_pf.name = this->pf_msg.name;
            this->pcl_pf.offset = this->pf_msg.offset;
            this->pcl_pf.datatype = this->pf_msg.datatype;
            this->pcl_pf.count = this->pf_msg.count;
            this->pcl_pfs.push_back(this->pcl_pf);

        }
        
        // PCL STUFF: UNCOMMENT FOLLOWING LINE TO SAVE TO PCD -  populate pcl PointCloud with PointFields
        this->pcl_pc2.fields = this->pcl_pfs;

        ptr += size_t(121);
        std::memcpy(&this->msg.is_bigendian, &data[ptr], sizeof(this->msg.is_bigendian));
        //std::cout << "Is Bigendian?: " <<  int(this->msg.is_bigendian) <<  std::endl;

        ptr += sizeof(this->msg.is_bigendian);
        std::memcpy(&this->msg.point_step, &data[ptr], sizeof(this->msg.point_step));
        //std::cout << "Point Step: " <<  this->msg.point_step << std::endl;

        ptr += sizeof(this->msg.point_step);
        std::memcpy(&this->msg.row_step, &data[ptr], sizeof(this->msg.row_step));
        //std::cout << "Row Step: " <<  this->msg.row_step << std::endl;

        ptr += sizeof(this->msg.row_step);
        
        long data_size;
        std::memcpy(&data_size, &data[ptr], sizeof(data_size));
        ptr += sizeof(data_size);
        this->msg.data = (uint8_t*)malloc((size_t)data_size);
        std::memcpy(&this->msg.data[0], &data[ptr], (size_t)(data_size));

        ptr += data_size;
        std::memcpy(&this->msg.is_dense, &data[ptr], sizeof(this->msg.is_dense));
        //std::cout << "Is Dense?: " <<  int(this->msg.is_dense) << std::endl;

        // PCL STUFF: UNCOMMENT FOLLOWING LINES TO SAVE TO PCD - populate pcl data
        this->pcl_pc2.is_bigendian = this->msg.is_bigendian;
        this->pcl_pc2.point_step = this->msg.point_step;
        this->pcl_pc2.row_step = this->msg.row_step;
        this->pcl_pc2.is_dense = this->msg.is_dense;
       
        this->pcl_pc2.data.clear();
        for (int i=0; i<data_size+1; i++){
            this->pcl_pc2.data.push_back(this->msg.data[i]);
        }


        std::cout << "DONE DESERIALIZING" << std::endl;

        delete [] data;

        return this->msg;
    }

    void DeserializerLivox::process_message(struct BaseMsg* msg_){

        //PCL STUFF: UNCOMMENT FOLLOWING LINES TO SAVE TO PCD - save pcd using PCL library
        pcl::PointCloud<pcl::PointXYZI>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZI>);
        pcl::fromPCLPointCloud2(this->pcl_pc2,*temp_cloud);
        std::string timestr = std::to_string(this->msg.timestamp_sec) + std::to_string(this->msg.timestamp_nanosec);
        std::string pcd_name = this->out_path + timestr + ".pcd"; 
        //std::cout << "PCD NAME: " << pcd_name << std::endl;
        pcl::io::savePCDFileASCII(pcd_name, *temp_cloud);

        std::cout << "PCD Saved" << std::endl;
    }

}}} // namespace m2s2, ecal, deserializer
