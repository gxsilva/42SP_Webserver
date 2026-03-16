/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenType.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:53:54 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:22:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENTYPE_HPP
#define TOKENTYPE_HPP

// namespace TokenType
// {
// 	enum Type
// 	{
// 		// Literais
// 		WORD,	// server, listen, location, root, etc.
// 		NUMBER, // 8002, 404, 3000000
// 		STRING, // "index.html", '/tours', valores entre aspas
// 		PATH,	// /tours, /cgi-bin, ./docs (caminhos sem aspas)

// 		// Delimitadores estruturais
// 		LBRACE,	   // {
// 		RBRACE,	   // }
// 		SEMICOLON, // ;
// 		SLASH,	   // / (quando usado sozinho)

// 		// Especiais
// 		NEWLINE,   // \n (opcional, útil para debugging)
// 		EOF_TOKEN, // fim do arquivo
// 		UNKNOWN	   // token não reconhecido (erro)
// 	};
// }

enum TokenType
{
	// Literais
	WORD,	// server, listen, location, root, etc.
	NUMBER, // 8002, 404, 3000000
	STRING, // "index.html", '/tours', valores entre aspas
	PATH,	// /tours, /cgi-bin, ./docs (caminhos sem aspas)

	// Delimitadores estruturais
	LBRACE,	   // {
	RBRACE,	   // }
	SEMICOLON, // ;
	SLASH,	   // / (quando usado sozinho)

	// Especiais
	NEWLINE,   // \n (opcional, útil para debugging)
	EOF_TOKEN, // fim do arquivo
	UNKNOWN	   // token não reconhecido (erro)
};

// typedef TokenType::Type TokenType;

// class TokenType
// {
// 	private:
// 		TokenType();
// 		~TokenType();
// 	public:
// 		enum Type
// 		{
// 			// Literais
// 			WORD,	// server, listen, location, root, etc.
// 			NUMBER, // 8002, 404, 3000000
// 			STRING, // "index.html", '/tours', valores entre aspas
// 			PATH,	// /tours, /cgi-bin, ./docs (caminhos sem aspas)

// 			// Delimitadores estruturais
// 			LBRACE,	   // {
// 			RBRACE,	   // }
// 			SEMICOLON, // ;
// 			SLASH,	   // / (quando usado sozinho)

// 			// Especiais
// 			NEWLINE,   // \n (opcional, útil para debugging)
// 			EOF_TOKEN, // fim do arquivo
// 			UNKNOWN	   // token não reconhecido (erro)
// 		};
// };

#endif /* TOKENTYPE_HPP */
