

#include <thread>
#include <chrono>

#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include "dds_sdkPubSubTypes.h"

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::rtps;

class DDSListenerStat {
public:
    std::atomic_uint64_t _currentMatchedSubs = 0;
    std::atomic_uint64_t _cumulativeMatchedSubs = 0;
    std::atomic_uint64_t _totalPubOkDatas = 0;
    std::atomic_uint64_t _totalPubFailedDatas = 0;
    std::atomic_uint64_t _currentMatchedPubs = 0;
    std::atomic_uint64_t _cumulativeMatchedPubs = 0;
    std::atomic_uint64_t _totalSubDroppedDatas = 0;
    std::atomic_uint64_t _totalSubLostDatas = 0;
    std::atomic_uint64_t _totalSubValidDatas = 0;
    uint16_t _totalPubTopics = 0;
    uint16_t _totalSubTopics = 0;

    int64_t _statTimeStart = std::chrono::steady_clock::now().time_since_epoch().count();

    void printStat() {
        // std::unique_lock xl(_mutex); // We just want a rough statistical results
        double totalSecs = (std::chrono::steady_clock::now().time_since_epoch().count() - _statTimeStart) * 1.0E-9 + 1E-3;
        if (_totalPubTopics > 0) {
            auto totalPubDatas = _totalPubOkDatas + _totalPubFailedDatas;
            double pubDPS = 1E-3 * totalPubDatas / totalSecs; // kilo datas per seconds
            double pubOkRatio = 100 * _totalPubOkDatas / (totalPubDatas > 0 ? totalPubDatas : 1);
            EPROSIMA_LOG_WARNING(Test, "##pub stat totalSeconds:" << totalSecs << "s,totalTopics:" << _totalPubTopics
                << ",_currentMatchedSubs:" << _currentMatchedSubs << ",_cumulativeMatchedSubs:" << _cumulativeMatchedSubs << ",_totalPubOkDatas:" << _totalPubOkDatas << ",_totalPubFailedDatas:" << _totalPubFailedDatas << ",pubOkRatio:" << pubOkRatio << "%,pubDPS:" << pubDPS << "k/s");
        }
        if (_totalSubTopics > 0) {
            auto totalSubDatas = _totalSubDroppedDatas + _totalSubValidDatas;
            double subDPS = 1E-3 * totalSubDatas / totalSecs; // kilo datas per seconds
            double subOkRatio = 100 * _totalSubValidDatas / (totalSubDatas > 0 ? totalSubDatas : 1);
            double subLostRatio = 100 * _totalSubLostDatas / ((totalSubDatas > 0 ? totalSubDatas : 1) + _totalSubLostDatas);
            EPROSIMA_LOG_WARNING(Test, "##sub stat totalSeconds:" << totalSecs << "s,totalTopics:" << _totalSubTopics
                << ",_currentMatchedPubs:" << _currentMatchedPubs << ",_cumulativeMatchedPubs:" << _cumulativeMatchedPubs << ",_totalSubValidDatas:" << _totalSubValidDatas << ",_totalSubDroppedDatas:" << _totalSubDroppedDatas << ",subOkRatio:" << subOkRatio << "%,subLostRatio:"<<subLostRatio << ",subDPS:" << subDPS << "k/s");
        }
    }
};

class ParticipantListener : public DomainParticipantListener
{
public:
    inline void on_participant_discovery(DomainParticipant* participant, ParticipantDiscoveryInfo&& info) override
    {
        switch (info.status)
        {
        case ParticipantDiscoveryInfo::DISCOVERED_PARTICIPANT:
            EPROSIMA_LOG_WARNING(Test, "Discovered New DomainParticipant(name:" << info.info.m_participantName << ",handle:" << info.info.m_key
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle()
                << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ParticipantDiscoveryInfo::REMOVED_PARTICIPANT:
            EPROSIMA_LOG_WARNING(Test, "Removed A DomainParticipant(name:" << info.info.m_participantName << ",handle:" << info.info.m_key
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle()
                << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ParticipantDiscoveryInfo::CHANGED_QOS_PARTICIPANT:
            EPROSIMA_LOG_WARNING(Test, "Qos Changed of A DomainParticipant(name:" << info.info.m_participantName << ",handle:" << info.info.m_key
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle()
                << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ParticipantDiscoveryInfo::DROPPED_PARTICIPANT:
            EPROSIMA_LOG_WARNING(Test, "Dropped A DomainParticipant(name:" << info.info.m_participantName << ",handle:" << info.info.m_key
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle()
                << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ParticipantDiscoveryInfo::IGNORED_PARTICIPANT:
            EPROSIMA_LOG_WARNING(Test, "Ignored A DomainParticipant(name:" << info.info.m_participantName << ",handle:" << info.info.m_key
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle()
                << ",domainId:" << participant->get_domain_id() << ")");
            break;
        }
    }
    inline virtual void on_publisher_discovery(DomainParticipant* participant, WriterDiscoveryInfo&& info) override
    {
        switch (info.status)
        {
        case WriterDiscoveryInfo::DISCOVERED_WRITER:
            EPROSIMA_LOG_WARNING(Test, "Discovered New DataWriter(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case WriterDiscoveryInfo::CHANGED_QOS_WRITER:
            EPROSIMA_LOG_WARNING(Test, "Qos Changed of A DataWriter(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case WriterDiscoveryInfo::REMOVED_WRITER:
            EPROSIMA_LOG_WARNING(Test, "Removed A DataWriter(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case WriterDiscoveryInfo::IGNORED_WRITER:
            EPROSIMA_LOG_WARNING(Test, "Ignored A DataWriter(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        }
    }
    inline virtual void on_subscriber_discovery(DomainParticipant* participant, ReaderDiscoveryInfo&& info) override
    {
        switch (info.status) {
        case ReaderDiscoveryInfo::DISCOVERED_READER:
            EPROSIMA_LOG_WARNING(Test, "Discovered New DataReader(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ReaderDiscoveryInfo::CHANGED_QOS_READER:
            EPROSIMA_LOG_WARNING(Test, "Qos Changed of A DataReader(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ReaderDiscoveryInfo::REMOVED_READER:
            EPROSIMA_LOG_WARNING(Test, "Removed A DataReader(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        case ReaderDiscoveryInfo::IGNORED_READER:
            EPROSIMA_LOG_WARNING(Test, "Ignored A DataReader(topic:" << info.info.topicName() << ",type:" << info.info.typeName() << ",userDefinedId:" << info.info.userDefinedId() << ",handle:" << info.info.key() << ",participantHandle:" << info.info.RTPSParticipantKey()
                << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
            break;
        }
    }
    inline virtual void on_type_discovery(DomainParticipant* participant, const SampleIdentity& request_sample_id,
        const eprosima::fastrtps::string_255& topic, const eprosima::fastrtps::types::TypeIdentifier* /*identifier*/,
        const eprosima::fastrtps::types::TypeObject* /*object*/, eprosima::fastrtps::types::DynamicType_ptr /*dyn_type*/) override
    {
        EPROSIMA_LOG_WARNING(Test, "Discoverd New data type(topic:" << topic << ",writerGuid:" << request_sample_id.writer_guid()
            << "),currentParticipant(name:" << participant->get_participant_names().front() << ",handle:" << participant->get_instance_handle() << ",domainId:" << participant->get_domain_id() << ")");
    }
};
class WriterListener : public DataWriterListener {
public:
    DDSListenerStat* stat;
    WriterListener(DDSListenerStat* stat_) : DataWriterListener(), stat(stat_){}
    inline virtual void on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info) override {
        if (info.current_count_change == 1)
        {
            ++stat->_currentMatchedSubs;
            ++stat->_cumulativeMatchedSubs;
            EPROSIMA_LOG_WARNING(Test, "DataWriter got New Matched! _currentMatching:" << stat->_currentMatchedSubs << ",_cumulativeMatched:" << stat->_cumulativeMatchedSubs << ",topic:"
                << writer->get_topic()->get_name() << ",writer_hanedle:" << writer->get_instance_handle() << ",subscription_handle:" << info.last_subscription_handle);
        }
        else if (info.current_count_change == -1)
        {
            --stat->_currentMatchedSubs;
            EPROSIMA_LOG_WARNING(Test, "DataWriter lost a Match! _currentMatching:" << stat->_currentMatchedSubs << ",_cumulativeMatched:" << stat->_cumulativeMatchedSubs << ",topic:"
                << writer->get_topic()->get_name() << ",writer_hanedle:" << writer->get_instance_handle() << ",subscription_handle:" << info.last_subscription_handle);
        }
        else
        {
            EPROSIMA_LOG_ERROR(Test, "DataWriter writer_hanedle:" << writer->get_instance_handle() << " current_count_change:'" << info.current_count_change << "' is not a valid value for PublicationMatchedStatus current_count_change!_currentMatching:" << stat->_currentMatchedSubs << ",_cumulativeMatched:" << stat->_cumulativeMatchedSubs);
        }
    }
    inline virtual void on_liveliness_lost(DataWriter* writer, const LivelinessLostStatus& status) override {
        EPROSIMA_LOG_WARNING(Test, "writer:writer_hanedle:" << writer->get_instance_handle() << " Matched Subscribers will consider us  offline!Writer on_liveliness_lost total:" << status.total_count << ",current:" << status.total_count_change);
    }
    inline virtual void on_offered_incompatible_qos(DataWriter* writer, const OfferedIncompatibleQosStatus& status) override
    {
        EPROSIMA_LOG_WARNING(Test, "writer:writer_hanedle:" << writer->get_instance_handle() << " Found a remote Topic with incompatible QoS (QoS ID: " << status.last_policy_id << ")");
    }
    inline virtual void on_offered_deadline_missed(DataWriter* writer, const OfferedDeadlineMissedStatus& status) override {
        EPROSIMA_LOG_WARNING(Test, "writer:" << writer->get_instance_handle() << " on_offered_deadline_missed total:" << status.total_count << ",current:" << status.total_count_change);
    }
};
class ReaderListener : public DataReaderListener{
public:
    DDSListenerStat* stat;
    ReaderListener(DDSListenerStat* stat_) : DataReaderListener(), stat(stat_){}
    inline virtual void on_data_available(DataReader* reader) override {
        FASTDDS_CONST_SEQUENCE(DataSeq, MeasurementValue);
        DataSeq loanSamples_;
        SampleInfoSeq loanInfos_;
        while (true)
        {
            ReturnCode_t returnCode = reader->take(loanSamples_, loanInfos_);
            if(returnCode != ReturnCode_t::RETCODE_OK){
                if(returnCode != ReturnCode_t::RETCODE_NO_DATA){
                    EPROSIMA_LOG_ERROR(Test, "Reader take failed! ReturnCode_t:" << returnCode() << ",topic:" << reader->get_topicdescription()->get_name() << ",reader:" << reader->guid());
                }
                reader->return_loan(loanSamples_, loanInfos_);
                break;
            }
            for (LoanableCollection::size_type j = 0, batchCount = loanInfos_.length(); j < batchCount; ++j)
            {
                if (!loanInfos_[j].valid_data)
                {
                    continue;
                }
                const auto &meaVal = loanSamples_[j];
                int64_t mvTimeNs = loanInfos_[j].source_timestamp.to_ns();
                switch (meaVal.v()._d())
                {
                case MK_ANALOG:
                case MK_DISCRETE:
                    ++stat->_totalSubValidDatas;
                    break;
                default:
                    EPROSIMA_LOG_WARNING(Test, "Dropped Unknow measurement kind:" << meaVal.v()._d() << ",rid:"<<meaVal.rid() << ",q:"<<meaVal.q()<<",t:"<<mvTimeNs<<"ns");
                    ++stat->_totalSubDroppedDatas;
                    break;
                }
            }
            reader->return_loan(loanSamples_, loanInfos_);
        }
    }
    inline virtual void on_subscription_matched(DataReader *reader, const SubscriptionMatchedStatus &info) override {
        if (info.current_count_change == 1)
        {
            ++stat->_currentMatchedPubs;
            ++stat->_cumulativeMatchedPubs;
            EPROSIMA_LOG_WARNING(Test, "DataReader got New Matched!currentMatching:" << info.current_count << ",cumulativeMatched:"
            << info.total_count << ",topic:"<< reader->get_topicdescription()->get_name()
            << ",reader_hanedle:"<<reader->get_instance_handle() << ",publication_handle:"
            << info.last_publication_handle);
        }
        else if (info.current_count_change == -1)
        {
            --stat->_currentMatchedPubs;
            EPROSIMA_LOG_WARNING(Test, "DataReader lost a Match!currentMatching:" << info.current_count << ",cumulativeMatched:"
            << info.total_count << ",topic:"
                << reader->get_topicdescription()->get_name()  << ",reader_hanedle:"<<reader->get_instance_handle()
                << ",publication_handle:" << info.last_publication_handle);
        }
        else
        {
            EPROSIMA_LOG_ERROR(Test, "DataReader reader_hanedle:" << reader->get_instance_handle() << " current_count_change:'" << info.current_count_change
                << "' is not a valid value for SubscriptionMatchedStatus current_count_change!currentMatching:"
                << info.current_count << ",cumulativeMatched:" << info.total_count);
        }
    }
    inline virtual void on_requested_incompatible_qos(DataReader *reader, const RequestedIncompatibleQosStatus &info) override {
        EPROSIMA_LOG_WARNING(Test, "DataReader incompatible_qos DataWriter! currentChange:" << info.total_count_change << ",total_count:" << info.total_count << ",topic:"
            << reader->get_topicdescription()->get_name() << ",reader_hanedle:" << reader->get_instance_handle()
            << ",last incompatible QosPolicyId_t:" << info.last_policy_id << ",totalQosPolicyCount:"
            << info.policies.size());
    }
    inline virtual void on_liveliness_changed(DataReader *reader, const LivelinessChangedStatus &info) override {
        if (info.alive_count_change == 1)
        {
            EPROSIMA_LOG_WARNING(Test, "matched DataWriter become Active! alive_count:" << info.alive_count <<",topic:"
                << reader->get_topicdescription()->get_name() << ",reader_hanedle:"<<reader->get_instance_handle()
                << ",publication_handle:" << info.last_publication_handle);
        }
        else if (info.not_alive_count_change == -1)
        {
            EPROSIMA_LOG_WARNING(Test, "DataReader matched DataWriter become Inactive!not_alive_count:" << info.not_alive_count << ",topic:"
                << reader->get_topicdescription()->get_name()  << ",reader_hanedle:"<<reader->get_instance_handle()
                << ",publication_handle:" << info.last_publication_handle);
        }
    }
    inline virtual void on_sample_lost(DataReader* reader, const SampleLostStatus& status) override {
        (void)(reader);
        (void)(status);
        ++stat->_totalSubLostDatas;
    }
    inline virtual void on_sample_rejected(DataReader* reader, const SampleRejectedStatus& status) override {
        ++stat->_totalSubLostDatas;
        EPROSIMA_LOG_WARNING(Test, "Received samples are Rejected!reason:" << static_cast<int>(status.last_reason)
            << ",rejectedCount"<<status.total_count_change << ",cumulativeCount:"<<status.total_count
            <<",topic:" << reader->get_topicdescription()->get_name() << ",reader_hanedle:"<<reader->get_instance_handle()
            << ",instance_handle:" << status.last_instance_handle);
    }
    inline virtual void on_requested_deadline_missed(DataReader *reader, const RequestedDeadlineMissedStatus &status) override {
        EPROSIMA_LOG_WARNING(Test, "Samples are Not Received On Time!missedCount"<<status.total_count_change << ",cumulativeCount:"<<status.total_count
            <<",topic:" << reader->get_topicdescription()->get_name() << ",reader_hanedle:"<<reader->get_instance_handle()
            << ",instance_handle:" << status.last_instance_handle);
    }
};
DomainParticipant* createParticipant(size_t topicCount, std::vector<Topic*>& topicSeq)
{
    DomainParticipantQos pqos = PARTICIPANT_QOS_DEFAULT;
    pqos.name("Participant_pub");
    auto factory = DomainParticipantFactory::get_instance();
    factory->load_profiles();
    factory->get_default_participant_qos(pqos);
    auto pListener_ = new ParticipantListener;
    DomainParticipant* participant_ = factory->create_participant(0, pqos, pListener_, StatusMask::none());
    TypeSupport type_{ new MeasurementValuePubSubType()};
    type_.register_type(participant_);
    TopicQos tqos = TOPIC_QOS_DEFAULT;
    participant_->get_default_topic_qos(tqos);
    for (size_t j = 0; j < topicCount; ++j)
    {
        std::string meaName{"Topic_"+std::to_string(j)};
        auto topic = participant_->create_topic(meaName, type_.get_type_name(), tqos);
        assert(topic != nullptr);
        topicSeq.push_back(topic);
    }
    const auto & pport = pqos.wire_protocol().port;
    printf("***sub participant: pName:%s,pId:%d,portBase:%u,metaUnicastPort:%u,userUnicastPort:%u,topicCount:%lu\n",pqos.name().c_str(),pqos.wire_protocol().participant_id,
        pport.portBase,pport.portBase+pport.offsetd1+2*pqos.wire_protocol().participant_id,
        pport.portBase+pport.offsetd3+2*pqos.wire_protocol().participant_id,topicCount);
    return participant_;
}
int testPub(DDSListenerStat* stat,uint16_t totalTopics = 1, uint32_t ridsPerTopic = 1, uint64_t totalDatas = 100,
        uint16_t threadCount = 1, uint32_t durationMs = 100 * 1.0E3, int publishMode = 1,DomainParticipant* participant = nullptr) {
    // 0:never write;1:write when first matching;>=2:write whitout waiting for any reader
    publishMode = publishMode < 1 ? 0 : publishMode;
    totalTopics = totalTopics > 0 ? totalTopics : 1; // all topics to publish data
    ridsPerTopic = ridsPerTopic > 0 ? ridsPerTopic : 1; // total instances (rid is key) of a topic;
    threadCount = threadCount > 0 ? threadCount : 1; // create multi threads to writ partial datas
    durationMs = durationMs > 0 ? durationMs : 1000; // to calculate sleep interval of wirte operation
    uint64_t writeCountPerTh = totalDatas / threadCount; // partial datas for a thread to write
    totalDatas = writeCountPerTh * threadCount;
    auto sleepNs = std::chrono::nanoseconds((uint64_t)(writeCountPerTh == 0 ? (durationMs * 1.0E6 / totalTopics) : (durationMs * 1.0E6 / writeCountPerTh)));
    EPROSIMA_LOG_WARNING(Pub, "topcisSize:" << totalTopics << ",ridsPerTopic:" << ridsPerTopic << ",totalDatas:" << totalDatas << ",  threadCount:"<<threadCount << ",writeCountPerTh:"<<writeCountPerTh << ",durationMs:" << durationMs << ",sleepNs:" << sleepNs.count() << ",publishMode:" << publishMode);
    std::vector<DataWriter*> writerSeq;
    std::vector<Topic*> topicSeq;
    std::vector<std::string> nameSeq;
    auto participant_ = participant ? participant : createParticipant(totalTopics, topicSeq);
    PublisherQos pubqos = PUBLISHER_QOS_DEFAULT;
    participant_->get_default_publisher_qos(pubqos);
    auto publisher_ = participant_->create_publisher(pubqos, nullptr);
    DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
    publisher_->get_default_datawriter_qos(wqos);
    auto wListener_ = new WriterListener(stat);
    stat->_totalPubTopics = topicSeq.size();
    for (auto topic : topicSeq) {
        auto writer = publisher_->create_datawriter(topic, wqos, wListener_);
        assert(writer != nullptr);
        writerSeq.push_back(writer);
        nameSeq.push_back(topic->get_name());
        writer->enable();
    }
    if (publishMode == 0) {
        EPROSIMA_LOG_WARNING(Pub, "publishMode ==0 !!Will Never publish data!!");
        while (publishMode == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    auto writeData = [=]()
    {
        EPROSIMA_LOG_WARNING(Pub, "starting WriteData....writeCountPerTh:"<<writeCountPerTh);
        uint64_t writeCount = 0, ridIndex = 0, writerIndex = 0;
        MeasurementValue mv;
        auto begin = std::chrono::steady_clock::now();
        while (writeCount < writeCountPerTh || writeCountPerTh==0)
        {
            mv.rid(ridIndex);
            mv.q(writerIndex);
            if (writerIndex % 2 ==0) {
                mv.v().av(threadCount+0.1);
            }
            else {
                mv.v().dv(threadCount);
            }
            auto t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            eprosima::fastrtps::Time_t mvTime((int32_t)(t * 1.0E-6), 0);
            mvTime.nanosec = (uint32_t)(t - mvTime.seconds * 1000000ULL)*1000;
            auto writer = writerSeq[writerIndex];
            auto rt = writer->write_w_timestamp((void *)(&mv), eprosima::fastdds::dds::HANDLE_NIL, mvTime);
            if (rt == ReturnCode_t::RETCODE_OK){
                ++stat->_totalPubOkDatas;
            }
            else {
                ++stat->_totalPubFailedDatas;
                EPROSIMA_LOG_ERROR(Pub, "writer->write_w_timestamp Failed!returnCode:" << rt() << ",rid:" << ridIndex << ",topic:"<<topicSeq[writerIndex]);
            }
            ++writeCount;
            if (++ridIndex >= ridsPerTopic) {
                ridIndex = 0;
            }
            if (++writerIndex >= totalTopics) {
                writerIndex = 0;
            }
            if (writeCount == 0) {
                begin = std::chrono::steady_clock::now() + sleepNs;
            }
            std::this_thread::sleep_until(begin + writeCount * sleepNs);
        }
    };
    while (true)
    {
        if (publishMode == 1 && stat->_cumulativeMatchedSubs == 0) {
            static int count = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if (++count % 10 == 0) {
                count = 0;
                EPROSIMA_LOG_WARNING(Pub, "No data writed when waiting for readers to match!");
            }
        } else {
            auto begin = std::chrono::steady_clock::now();
            stat->_statTimeStart = begin.time_since_epoch().count();
            std::vector<std::thread> thes;
            for (size_t i = 0; i < threadCount; i++)
            {
                thes.push_back(std::thread(writeData));
            }
            uint64_t sleepMs = 1000, statInterMs = 10 * 1E3, statLoops = 0;
            while (totalDatas == 0 || (stat->_totalPubFailedDatas + stat->_totalPubOkDatas) < totalDatas)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
                if ((++statLoops * sleepMs) >= statInterMs)
                {
                    stat->printStat();
                    statLoops = 0;
                }
            }
            for (auto& th : thes) {
                th.join();
            }
            break;
        }
    }
    participant_->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
    return 0;
}
int testSub(DDSListenerStat* stat,uint16_t totalTopics = 1, uint64_t waitingLoops = 10,DomainParticipant* participant = nullptr) {
    totalTopics = totalTopics > 0 ? totalTopics : 1; // all topics to subscribe data
    EPROSIMA_LOG_WARNING(Sub, "topcisSize:" << totalTopics << ",waitingLoops:" << waitingLoops);
    std::vector<Topic*> topicSeq;
    auto participant_ = participant ? participant : createParticipant(totalTopics, topicSeq);
    SubscriberQos subqos = SUBSCRIBER_QOS_DEFAULT;
    participant_->get_default_subscriber_qos(subqos);
    auto subscriber_ = participant_->create_subscriber(subqos, nullptr);
    DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
    subscriber_->get_default_datareader_qos(rqos);
    auto rListener_ = new ReaderListener(stat);
    stat->_totalSubTopics = topicSeq.size();
    for (auto topic : topicSeq) {
        auto reader = subscriber_->create_datareader(topic, rqos, rListener_);
        assert(reader != nullptr);
        reader->enable();
    }
    uint64_t sleepMs = 2000, statInterMs = 10 * 1E3, statLoops = 0, loops = 0;
    while (++loops < waitingLoops || waitingLoops == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        if ((++statLoops * sleepMs) >= statInterMs)
        {
            stat->printStat();
            statLoops = 0;
        }
    }
    participant_->delete_contained_entities();
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
    return 0;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << "arg(" << i << "," << argv[i] << ")" << std::endl;
    }
    Log::Log::ReportFilenames(true);
    Log::Log::SetVerbosity(Log::Log::Kind::Info);
    int rt = 0;
    auto stat = new DDSListenerStat;
    std::string target = argc > 1 ? argv[1] : "";
    uint16_t totalTopics = argc > 2 ? std::stoul(argv[2]) : 200;
    if (target == "sub") {
        uint64_t waitingLoops = argc > 3 ? std::stoull(argv[3]) : 0;
        rt = testSub(stat, totalTopics, waitingLoops);
    }
    else if(target == "pub") {
        uint32_t ridsPerTopic = argc > 3 ? std::stoul(argv[3]) : 10000;
        uint64_t totalDatas = argc > 4 ? std::stoull(argv[4]) : 0;
        uint16_t pubThreadCount = argc > 5 ? std::stoul(argv[5]) : 1;
        uint32_t durationMs = argc > 6 ? std::stoul(argv[6]) : 1;
        int publishMode = argc > 7 ? std::stoi(argv[7]) : 2;
        rt = testPub(stat, totalTopics, ridsPerTopic, totalDatas, pubThreadCount, durationMs, publishMode);
    }
    else {
        EPROSIMA_LOG_ERROR(Test, " Invalid args!usage:"
            << "\npublish test: ./test-dds pub <totalTopics:200><ridsPerTopic:10000> <totalDatas:0>  <pubThreadCount:1> <durationMs:1> <publishMode:2>"
            << "\nsubscribe test: ./test-dds sub <totalTopics:200> <waitingLoops:0> "
        );
    }
    EPROSIMA_LOG_WARNING(Test, "***  main exit!threadId:" << std::this_thread::get_id() << "rt:" << rt);
    return 0;
}
