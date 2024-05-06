// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file dds_sdkCdrAux.ipp
 * This source file contains some declarations of CDR related functions.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef _FAST_DDS_GENERATED_DDS_SDKCDRAUX_IPP_
#define _FAST_DDS_GENERATED_DDS_SDKCDRAUX_IPP_

#include "dds_sdkCdrAux.hpp"

#include <fastcdr/Cdr.h>
#include <fastcdr/CdrSizeCalculator.hpp>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

namespace eprosima {
namespace fastcdr {









template<>
eProsima_user_DllExport size_t calculate_serialized_size(
        eprosima::fastcdr::CdrSizeCalculator& calculator,
        const MValue& data,
        size_t& current_alignment)
{
    static_cast<void>(data);

    eprosima::fastcdr::EncodingAlgorithmFlag previous_encoding = calculator.get_encoding();
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(
                                eprosima::fastcdr::CdrVersion::XCDRv2 == calculator.get_cdr_version() ?
                                eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
                                eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                                current_alignment)};

    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(0), data._d(),
                    current_alignment);

    switch (data._d())
    {
        case MK_ANALOG:
                    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(1),
                                data.av(), current_alignment);
                    break;

        case MK_DISCRETE:
                    calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(2),
                                data.dv(), current_alignment);
                    break;

        default:
            break;
    }

    calculated_size += calculator.end_calculate_type_serialized_size(previous_encoding, current_alignment);

    return calculated_size;
}


template<>
eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& scdr,
        const MValue& data)
{
    eprosima::fastcdr::Cdr::state current_state(scdr);
    scdr.begin_serialize_type(current_state,
            eprosima::fastcdr::CdrVersion::XCDRv2 == scdr.get_cdr_version() ?
            eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
            eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR);

    scdr << eprosima::fastcdr::MemberId(0) << data._d();

    switch (data._d())
    {
                case MK_ANALOG:
                    scdr << eprosima::fastcdr::MemberId(1) << data.av();
                    break;

                case MK_DISCRETE:
                    scdr << eprosima::fastcdr::MemberId(2) << data.dv();
                    break;

        default:
            break;
    }

    scdr.end_serialize_type(current_state);
}

template<>
eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr,
        MValue& data)
{
    cdr.deserialize_type(eprosima::fastcdr::CdrVersion::XCDRv2 == cdr.get_cdr_version() ?
            eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
            eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
            [&data](eprosima::fastcdr::Cdr& dcdr, const eprosima::fastcdr::MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                    case 0:
                        dcdr >> data._d();
                        break;
                    default:
                        switch (data._d())
                        {
                                                        case MK_ANALOG:
                                                            dcdr >> data.av();
                                                            break;

                                                        case MK_DISCRETE:
                                                            dcdr >> data.dv();
                                                            break;

                            default:
                                break;
                        }
                        ret_value = false;
                        break;
                }
                return ret_value;
            });
}



template<>
eProsima_user_DllExport size_t calculate_serialized_size(
        eprosima::fastcdr::CdrSizeCalculator& calculator,
        const MeasurementValue& data,
        size_t& current_alignment)
{
    static_cast<void>(data);

    eprosima::fastcdr::EncodingAlgorithmFlag previous_encoding = calculator.get_encoding();
    size_t calculated_size {calculator.begin_calculate_type_serialized_size(
                                eprosima::fastcdr::CdrVersion::XCDRv2 == calculator.get_cdr_version() ?
                                eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
                                eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
                                current_alignment)};


        calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(0),
                data.rid(), current_alignment);

        calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(1),
                data.q(), current_alignment);

        calculated_size += calculator.calculate_member_serialized_size(eprosima::fastcdr::MemberId(2),
                data.v(), current_alignment);


    calculated_size += calculator.end_calculate_type_serialized_size(previous_encoding, current_alignment);

    return calculated_size;
}

template<>
eProsima_user_DllExport void serialize(
        eprosima::fastcdr::Cdr& scdr,
        const MeasurementValue& data)
{
    eprosima::fastcdr::Cdr::state current_state(scdr);
    scdr.begin_serialize_type(current_state,
            eprosima::fastcdr::CdrVersion::XCDRv2 == scdr.get_cdr_version() ?
            eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
            eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR);

    scdr
        << eprosima::fastcdr::MemberId(0) << data.rid()
        << eprosima::fastcdr::MemberId(1) << data.q()
        << eprosima::fastcdr::MemberId(2) << data.v()
;
    scdr.end_serialize_type(current_state);
}

template<>
eProsima_user_DllExport void deserialize(
        eprosima::fastcdr::Cdr& cdr,
        MeasurementValue& data)
{
    cdr.deserialize_type(eprosima::fastcdr::CdrVersion::XCDRv2 == cdr.get_cdr_version() ?
            eprosima::fastcdr::EncodingAlgorithmFlag::DELIMIT_CDR2 :
            eprosima::fastcdr::EncodingAlgorithmFlag::PLAIN_CDR,
            [&data](eprosima::fastcdr::Cdr& dcdr, const eprosima::fastcdr::MemberId& mid) -> bool
            {
                bool ret_value = true;
                switch (mid.id)
                {
                                        case 0:
                                                dcdr >> data.rid();
                                            break;

                                        case 1:
                                                dcdr >> data.q();
                                            break;

                                        case 2:
                                                dcdr >> data.v();
                                            break;

                    default:
                        ret_value = false;
                        break;
                }
                return ret_value;
            });
}

void serialize_key(
        eprosima::fastcdr::Cdr& scdr,
        const MeasurementValue& data)
{
    static_cast<void>(scdr);
    static_cast<void>(data);
                            scdr << data.rid();



}



} // namespace fastcdr
} // namespace eprosima

#endif // _FAST_DDS_GENERATED_DDS_SDKCDRAUX_IPP_
