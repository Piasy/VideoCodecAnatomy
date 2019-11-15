//
//  main.cpp
//  VideoCodecAnatomy
//
//  Created by Piasy on 2019/11/13.
//  Copyright © 2019 Piasy. All rights reserved.
//

#include <iostream>
#include <stdio.h>

#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/h264/h264_common.h"
#include "common_video/h265/h265_bitstream_parser.h"
#include "common_video/h265/h265_common.h"

void test_parse_h264() {
    webrtc::H264BitstreamParser parser;
    FILE* dump = fopen("/Users/piasy/Downloads/dump.h264", "rb");
    uint8_t buffer[1024 * 1024];
    memset(buffer, 0, sizeof(buffer));

    size_t read = 0;
    size_t remains = 0;
    while ((read = fread(buffer + remains, 1, sizeof(buffer) - remains, dump)) >
           0) {
        std::vector<webrtc::H264::NaluIndex> indices =
            webrtc::H264::FindNaluIndices(buffer, read);
        std::cout << "read " << indices.size() << " nalu" << std::endl;
        if (indices.size() > 0) {
            size_t remaining_pos = indices.back().payload_start_offset +
                                   indices.back().payload_size;
            for (int i = 0; i < indices.size(); i++) {
                size_t start = indices[i].start_offset;
                size_t end = (i == indices.size() - 1)
                                 ? remaining_pos
                                 : indices[i + 1].start_offset;
                parser.ParseBitstream(buffer + start, end - start);
                absl::optional<int> qp = parser.GetLastSliceQp();
                if (qp) {
                    std::cout << "last QP: " << *qp << std::endl;
                }
            }
            if (remaining_pos < sizeof(buffer)) {
                remains = sizeof(buffer) - remaining_pos;
                memcpy(buffer, buffer + remaining_pos, remains);
                std::cout << "remains " << remains << " bytes" << std::endl;
            }
        }
    }

    fclose(dump);
}

void test_parse_h265() {
    webrtc::H265BitstreamParser parser;
    FILE* dump = fopen("/Users/piasy/Downloads/dump.h265", "rb");
    uint8_t buffer[1024 * 1024];
    memset(buffer, 0, sizeof(buffer));

    size_t read = 0;
    size_t remains = 0;
    int32_t parsed_nalu_num = 0;
    while ((read = fread(buffer + remains, 1, sizeof(buffer) - remains, dump)) >
           0) {
        std::vector<webrtc::H265::NaluIndex> indices =
            webrtc::H265::FindNaluIndices(buffer, read);
        std::cout << "read " << indices.size() << " nalu" << std::endl;
        if (indices.size() > 0) {
            size_t remaining_pos = indices.back().payload_start_offset +
                                   indices.back().payload_size;
            for (int i = 0; i < indices.size(); i++) {
                size_t start = indices[i].start_offset;
                size_t end = (i == indices.size() - 1)
                                 ? remaining_pos
                                 : indices[i + 1].start_offset;
                parser.ParseBitstream(buffer + start, end - start);
                parsed_nalu_num++;
                absl::optional<int> qp = parser.GetLastSliceQp();
                if (qp) {
                    std::cout << "NALU " << parsed_nalu_num << " QP: " << *qp << std::endl;
                }
            }
            if (remaining_pos < sizeof(buffer)) {
                remains = sizeof(buffer) - remaining_pos;
                memcpy(buffer, buffer + remaining_pos, remains);
                std::cout << "remains " << remains << " bytes" << std::endl;
            }
        }
    }

    fclose(dump);
}

int main(int argc, const char* argv[]) {
    //test_parse_h264();
    test_parse_h265();
    return 0;
}
