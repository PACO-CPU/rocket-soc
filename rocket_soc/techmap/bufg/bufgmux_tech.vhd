-----------------------------------------------------------------------------
-- Entity:      System reset former
-- File:        tap_rstn.vhd
-- Author:      Sergey Khabarov - GNSS Sensor Ltd
-- Description: 
------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

library techmap;
use techmap.gencomp.all;

entity bufgmux_tech is
  generic
  (
    tech : integer := 0
  );
  port (
    O        : out std_ulogic;
    I1       : in std_ulogic;
    I2       : in std_ulogic;
    S        : in std_ulogic
    );
end;


architecture rtl of bufgmux_tech is

 component bufgmux_fpga is
  port (
    O       : out std_ulogic;
    I1      : in std_ulogic;
    I2      : in std_ulogic;
    S       : in std_ulogic
    );
  end component;

 component bufgmux_micron180 is
  port (
    O       : out std_ulogic;
    I1      : in std_ulogic;
    I2      : in std_ulogic;
    S       : in std_ulogic
    );
  end component;


begin


   inf : if tech = inferred generate
      O <= I1 when S = '0' else I2;
   end generate;

   xlnx : if tech = virtex6 or tech = kintex7 generate
      mux_buf : bufgmux_fpga
      port map (
        O   => O,
        I1  => I1,
        I2  => I2,
        S   => S
      );
   end generate;

   mic0 : if tech = micron180 generate
      mux_buf : bufgmux_micron180
      port map (
        O   => O,
        I1  => I1,
        I2  => I2,
        S   => S
      );
   end generate;

  
end;  
