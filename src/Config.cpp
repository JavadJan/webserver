/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 10:57:07 by asemykin          #+#    #+#             */
/*   Updated: 2025/12/21 11:03:01 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Config.hpp"

Config::Config()
{}

Config::Config(const Config &copy)
{
    *this = copy;
}

Config &Config::operator=(const Config &copy)
{
    if(this != &copy)
    {
           ports = copy.ports;
           root = copy.root;
           errorPages = copy.errorPages;  
    }
    
    return *this;
}

Config::~Config()
{}

