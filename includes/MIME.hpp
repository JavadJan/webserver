/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MIME.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 10:19:59 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/08 13:56:02 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "Config.hpp"

class MIME
{
    private:
        std::map<std::string, std::string> _mimeType;
    public:    
        MIME();
        std::string getSuffix(const std::string path);
        std::string getMimeType(const std::string suffix);
};
