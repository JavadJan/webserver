/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MIME.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 10:19:59 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/18 17:49:00 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include <iostream>

class MIME
{
    private:
        std::map<std::string, std::string> _mimeType;
    public:    
        MIME();
        std::string getSuffix(const std::string path);
        std::string getMimeType(const std::string suffix);
};
