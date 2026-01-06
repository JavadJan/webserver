/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 18:29:27 by asemykin          #+#    #+#             */
/*   Updated: 2025/12/20 19:01:40 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include <map>

class Config
{
    private:
        std::vector<int>            ports;       // dynamic array
        std::string                 root;
        std::map<int, std::string>  errorPages;  // key value container
        
        void parseFile(const std::string &filename);
        
    public:    
        Config();
        Config(const Config &copy);
        Config &operator=(const Config &copy);
        ~Config();

        std::vector<int>    getPorts() const;
        std::string         getRoot() const;
        std::string         getErrorPage(int errorCode) const;
};
