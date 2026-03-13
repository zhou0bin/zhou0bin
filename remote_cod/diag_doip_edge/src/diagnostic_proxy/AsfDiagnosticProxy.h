/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *            This software is copyright protected and proprietary to Neusoft Reach.
 *            Neusoft Reach grants to you only those rights as set out in the license conditions.
 *            All other rights remain with Neusoft Reach.
 *  -------------------------------------------------------------------------------------------------------------------
 *********************************************************************************************************************/


#include "ara/phm/common.h"
#include "cstring"






namespace ara {
namespace phm {
namespace supervised_entities {

namespace AsfDiagnosticProxy
{


enum class Checkpoints : uint32_t
{
    CP1 = 1U,
};


}//AsfDiagnosticProxy

} // namespace supervised_entities
} // namespace phm
} // namespace ara

const ara::phm::Checkpoint AsfDiagnosticProxy_CP1 = 1U;
const std::string instanceId_SE_AsfDiagnosticProxy_AsfDiagnosticProxy_RPort_PHM = "NeuSAR/AsfDiagnosticProxy/AsfDiagnosticProxy/RPort_PHM";



