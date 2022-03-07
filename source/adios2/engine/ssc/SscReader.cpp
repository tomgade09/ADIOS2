/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * SscReader.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: Jason Wang
 */

#include "SscReader.h"
#include "SscReaderGeneric.h"
#include "adios2/helper/adiosCommMPI.h"
#include "adios2/helper/adiosString.h"
#include <adios2-perfstubs-interface.h>

namespace adios2
{
namespace core
{
namespace engine
{

SscReader::SscReader(IO &io, const std::string &name, const Mode mode,
                     helper::Comm comm)
: Engine("SscReader", io, name, mode, std::move(comm))
{
    PERFSTUBS_SCOPED_TIMER_FUNC();

    helper::GetParameter(m_IO.m_Parameters, "Verbose", m_Verbosity);
    helper::GetParameter(m_IO.m_Parameters, "EngineMode", m_EngineMode);

    if (m_EngineMode == "generic")
    {
        m_EngineInstance = std::make_shared<ssc::SscReaderGeneric>(
            io, name, mode, CommAsMPI(m_Comm));
    }
    else if (m_EngineMode == "naive")
    {
    }
}

StepStatus SscReader::BeginStep(StepMode stepMode, const float timeoutSeconds)
{
    PERFSTUBS_SCOPED_TIMER_FUNC();

    helper::Log("Engine", "SSCReader", "BeginStep",
                std::to_string(CurrentStep()), 0, m_Comm.Rank(), 5, m_Verbosity,
                helper::LogMode::INFO);

    return m_EngineInstance->BeginStep(stepMode, timeoutSeconds,
                                       m_ReaderSelectionsLocked);
}

size_t SscReader::CurrentStep() const
{
    return m_EngineInstance->CurrentStep();
}

void SscReader::PerformGets()
{
    PERFSTUBS_SCOPED_TIMER_FUNC();
    m_EngineInstance->PerformGets();
}

void SscReader::EndStep()
{
    PERFSTUBS_SCOPED_TIMER_FUNC();

    helper::Log("Engine", "SSCReader", "EndStep", std::to_string(CurrentStep()),
                0, m_Comm.Rank(), 5, m_Verbosity, helper::LogMode::INFO);

    return m_EngineInstance->EndStep(m_ReaderSelectionsLocked);
}

void SscReader::DoClose(const int transportIndex)
{
    PERFSTUBS_SCOPED_TIMER_FUNC();

    helper::Log("Engine", "SSCReader", "Close", m_Name, 0, m_Comm.Rank(), 5,
                m_Verbosity, helper::LogMode::INFO);

    m_EngineInstance->Close(transportIndex);
}

#define declare_type(T)                                                        \
    void SscReader::DoGetSync(Variable<T> &variable, T *data)                  \
    {                                                                          \
        PERFSTUBS_SCOPED_TIMER_FUNC();                                         \
        helper::Log("Engine", "SSCReader", "GetSync", variable.m_Name, 0,      \
                    m_Comm.Rank(), 5, m_Verbosity, helper::LogMode::INFO);     \
        m_EngineInstance->GetDeferred(variable, data);                         \
        m_EngineInstance->PerformGets();                                       \
    }                                                                          \
    void SscReader::DoGetDeferred(Variable<T> &variable, T *data)              \
    {                                                                          \
        PERFSTUBS_SCOPED_TIMER_FUNC();                                         \
        helper::Log("Engine", "SSCReader", "GetDeferred", variable.m_Name, 0,  \
                    m_Comm.Rank(), 5, m_Verbosity, helper::LogMode::INFO);     \
        m_EngineInstance->GetDeferred(variable, data);                         \
    }                                                                          \
    std::vector<typename Variable<T>::BPInfo> SscReader::DoBlocksInfo(         \
        const Variable<T> &variable, const size_t step) const                  \
    {                                                                          \
        return m_EngineInstance->BlocksInfo(variable, step);                   \
    }
ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type

} // end namespace engine
} // end namespace core
} // end namespace adios2
