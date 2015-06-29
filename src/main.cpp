/**
 *  Copyright (C) 2015  Andrew Kallmeyer <fsmv@sapium.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the Lesser GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include "webhooks.h"
#include "telegram.h"

static const std::string CONFIG_FILE = "config.json";
static json config;

static bool loadConfig() {
    std::ifstream f(CONFIG_FILE);
    if(!f.good()) {
        return false;
    }
    
    std::istream_iterator<char> it(f), eof;
    std::string config_json(it, eof);
    config = json::parse(config_json);
    
    if (config.find("token") == config.end() ||
        config.find("api_url") == config.end() ||
        config.find("webhook_url") == config.end()) {
        return false;        
    }
    
    return true;
}

const json &getConfigOption(const std::string &option) {
    return config[option];
}

void luaHello() {
    lua_State *L = luaL_newstate();
    
    luaL_openlibs(L);
    luaL_dostring(L, "print('hello, '.._VERSION)");
    
    lua_close(L);
}

int main(int argc, char **argv) {
    if(!loadConfig()) {
        std::cerr << "Could not load config" << std::endl;
        return 1;
    }
    
    //luaHello();
    setWebhook(config["webhook_url"].get<std::string>());
    
    startServer(atoi(getenv("PORT")), getenv("IP"));
    std::cout << "Press Enter to stop" << std::endl;
    std::cin.ignore();
    stopServer();
    
    return 0;
}