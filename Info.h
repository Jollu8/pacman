#pragma once
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

class Info
{
public:
    Info(const std::string& text, const unsigned size, const sf::Vector2f position, const bool visible = true):
            m_visible(visible)
    {
        m_text.setString(text);
        m_text.setCharacterSize(size);
        m_text.setPosition(position);
        m_text.setFillColor(sf::Color::White);
    }


    void Render(sf::RenderWindow& window) const{
        if (m_visible)
        {
            window.draw(m_text);
        }
    }

    void SetString(const std::string& string){
        if (m_text.getString() != string) m_text.setString(string);
    }

    const sf::Vector2f& GetPosition() const{
        return m_text.getPosition();
    }

    void SetPosition(const sf::Vector2f& position){
        m_text.setPosition(position);
    }


    void SetFont(const sf::Font& font){
        m_text.setFont(font);
    }


    [[maybe_unused]] bool GetVisible() const{
        return m_visible;
    }

    void SetVisible(bool visibility){
        m_visible = visibility;
    }

private:
    sf::Text m_text;
    bool m_visible;
};