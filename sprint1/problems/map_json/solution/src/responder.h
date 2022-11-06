#pragma once
#include "responce_storage.h"
#include "response_command_chain.h"
#include <boost/beast/http.hpp>

namespace responder{
    namespace http = boost::beast::http;

    const auto RESPONDER = rcChain::ResponseCommandChain::CreateResponseCommandChain(responceStorage::MakeGetMapListResponse,
        {
            {responceStorage::MakeGetMapListResponseActivator, 
                {
                    {http::verb::get, responceStorage::MakeGetMapListResponse}
                }
            },
            {responceStorage::MakeGetMapByIdResponseActivator,
                {
                    {http::verb::get, responceStorage::MakeGetMapByIdResponse}
                }
            }
        });

}