// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofx/JSONRPC/Response.h"
#include "ofx/JSONRPC/JSONRPCUtils.h"


namespace ofx {
namespace JSONRPC {


const std::string Response::ERROR_TAG = "error";
const std::string Response::RESULT_TAG = "result";


Response::Response(HTTP::ServerEventArgs& evt):
    BaseMessage(evt, Json::Value::null),
    _result(Json::Value::null),
    _error(Error(Errors::RPC_ERROR_INTERNAL_ERROR))
{
}


Response::Response(HTTP::ServerEventArgs& evt,
                   const Json::Value& id,
                   const Json::Value& result):
    BaseMessage(evt, id),
    _result(result),
    _error(Error())
{
}


Response::Response(HTTP::ServerEventArgs& evt,
                   const Json::Value& id,
                   const Error& error):
    BaseMessage(evt, id),
    _result(Json::Value()),
    _error(error)
{
}


Response::~Response()
{
}


const Json::Value& Response::getResult() const
{
    return _result;
}


const Error& Response::getError() const
{
    return _error;
}


bool Response::isErrorResponse() const
{
    return Errors::RPC_ERROR_NONE != getError().getCode();
}


std::string Response::toString(bool styled) const
{
    return JSONRPCUtils::toString(toJSON(*this), styled);
}


Json::Value Response::toJSON(const Response& response)
{
    Json::Value result;

    result[PROTOCOL_VERSION_TAG] = PROTOCOL_VERSION;

    result["id"] = response.getId();

    if (response.isErrorResponse())
    {
        result["error"] = Error::toJSON(response.getError());
    }
    else
    {
        result["result"] = response.getResult();
    }

    return result;
}


Response Response::fromJSON(HTTP::ServerEventArgs& evt, const Json::Value& json)
{
    if (json.isMember(PROTOCOL_VERSION_TAG) &&
        json[PROTOCOL_VERSION_TAG].isString() && // require string
        json[PROTOCOL_VERSION_TAG].asString() == PROTOCOL_VERSION)
    {
        if (json.isMember(ID_TAG))
        {
            if (json.isMember(RESULT_TAG))
            {
                return Response(evt, json[ID_TAG], json[RESULT_TAG]);
            }
            else if(json.isMember(ERROR_TAG))
            {
                return Response(evt, json[ID_TAG], Error::fromJSON(json[ERROR_TAG]));
            }
            else
            {
                throw ParseException("No result OR error.");
            }
        }
        else
        {
            throw ParseException("No ID.");
        }
    }
    else
    {
        throw ParseException("No version string.");
    }
}


} } // namespace ofx::JSONRPC
