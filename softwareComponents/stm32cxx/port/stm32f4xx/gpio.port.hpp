#pragma once

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_ll_system.h>

#include <system/util.hpp>

#include <cassert>

namespace detail {

template < typename Self >
class Gpio {
public:
    Self& self() {
        return *static_cast< Self * >( this );
    }
    const Self& self() const {
        return *static_cast< Self * >( this );
    }

    void enableClock() {
        #ifdef GPIOA
            if ( self()._periph == GPIOA )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOA );
        #endif
        #ifdef GPIOB
            if ( self()._periph == GPIOB )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOB );
        #endif
        #ifdef GPIOC
            if ( self()._periph == GPIOC )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOC );
        #endif
        #ifdef GPIOD
            if ( self()._periph == GPIOD )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOD );
        #endif
        #ifdef GPIOE
            if ( self()._periph == GPIOE )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOE );
        #endif
        #ifdef GPIOF
            if ( self()._periph == GPIOF )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOF );
        #endif
        #ifdef GPIOG
            if ( self()._periph == GPIOG )
                return LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_GPIOG );
        #endif

        assert(false && "Unsupported GPIO");
    }

    void disableClock() {
        #ifdef GPIOA
            if ( self()._periph == GPIOA )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOA );
        #endif
        #ifdef GPIOB
            if ( self()._periph == GPIOB )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOB );
        #endif
        #ifdef GPIOC
            if ( self()._periph == GPIOC )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOC );
        #endif
        #ifdef GPIOD
            if ( self()._periph == GPIOD )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOD );
        #endif
        #ifdef GPIOE
            if ( self()._periph == GPIOE )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOE );
        #endif
        #ifdef GPIOF
            if ( self()._periph == GPIOF )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOF );
        #endif
        #ifdef GPIOG
            if ( self()._periph == GPIOG )
                return LL_AHB1_GRP1_DisableClock( LL_AHB1_GRP1_PERIPH_GPIOG );
        #endif

        assert(false && "Unsupported GPIO");
    }

    static IRQn_Type _positionToInterrupt( int pos ) {
        if ( pos >= 5 && pos <= 9 )
            return EXTI9_5_IRQn;
        if ( pos >= 10 )
            return EXTI15_10_IRQn;
        const IRQn_Type isr[]{
            EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn
        };
        return isr[ pos ];
    }

    static void _handleIrq( int line ) {
        assert( false );
        // TBA
    }

    static uint32_t _extiConfigLine( int pos ) {
        static const constexpr uint32_t table[ 16 ] = {
            LL_SYSCFG_EXTI_LINE1,
            LL_SYSCFG_EXTI_LINE2,
            LL_SYSCFG_EXTI_LINE3,
            LL_SYSCFG_EXTI_LINE4,
            LL_SYSCFG_EXTI_LINE5,
            LL_SYSCFG_EXTI_LINE6,
            LL_SYSCFG_EXTI_LINE7,
            LL_SYSCFG_EXTI_LINE8,
            LL_SYSCFG_EXTI_LINE9,
            LL_SYSCFG_EXTI_LINE10,
            LL_SYSCFG_EXTI_LINE11,
            LL_SYSCFG_EXTI_LINE12,
            LL_SYSCFG_EXTI_LINE13,
            LL_SYSCFG_EXTI_LINE14,
            LL_SYSCFG_EXTI_LINE15
        };
        return table[ pos ];
    }

    void setExtiSource( int line ) {
        int portIdx = indexOf( self()._periph, self().availablePeriphs );
        // PortIdx directly maps to PORT definition - this is a specialty of
        // this family
        LL_SYSCFG_SetEXTISource( portIdx, _extiConfigLine( line ) );
    }

protected:
    static uint32_t _getAdcChannel( GPIO_TypeDef */*port*/, int /*pos*/ ) {
        assert( false && "Not implemented" );
        __builtin_trap();
    }

    static ADC_TypeDef *_getAdcPeriph( GPIO_TypeDef */*port*/, int /*pos*/ ) {
        assert( false && "Not implemented" );
        __builtin_trap();
    }
};


} // namespace detail