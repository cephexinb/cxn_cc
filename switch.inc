$casflg equ <>
    $casvar equ <>
    $casstk equ <>
    
    switch macro _var:req, _reg:=<eax>
        mov _reg, _var
        $casstk catstr <_reg>, <#>, $casflg, <#>, $casstk
        $casvar equ _reg
        $casflg equ <0>         ;; 0 = emit an .if, 1 an .elseif
    endm
    
    case macro _args:vararg     ;; like Pascal: case id1. id4 .. id8, lparam, ...
                                ;; does an or (case1 || case2 || case3...)
      $cas textequ <>
      irp $v, <_args>         ;; for each case
          t@ instr <$v>, <..> ;; range ?
          if t@               ;; yes
              $LB substr <$v>, 1, t@-1                  ;; lbound = left portion
              $LB catstr <(>, $casvar, <!>=>, $LB, <)>  ;; ($casvar >= lbound)
              $UB substr <$v>, t@+2                     ;; ubound = right portion
              $UB catstr <(>, $casvar, <!<=>, $UB, <)>  ;; ($casvar <= ubound)
              $t catstr <(>, $LB, <&&> , $UB,<)>        ;; (($casvar >= $lb) && ($casvar <= $ub))
          else    ;; no, it's a value (var/const)
              $t catstr <(>, $casvar, <==>, <$v>, <)>   ;; ($casvar == value)
          endif
          $cas catstr <|| >, $t, $cas                   ;; or this case w/ others
      endm
      $cas substr $cas, 3 ;; lose the extra "|| " in front
        ifidn $casflg, <0> ;; 0 = 1'st case
            % .if $cas ;; emit ".if"
        else ;; all others
            % .elseif $cas ;; emit ".elseif"
        endif
        $casflg equ <1> ;; NOT 1'st
    endm
    
    default macro _default:vararg
        .else
        _default
    endm
    
    endsw macro _cmd:vararg
        ifidn $casstk, <>
            .err <Endsw w/o Switch>
        else
            t@ instr $casstk, <#>
            $casvar substr $casstk, 1, t@-1
            $casstk substr $casstk, t@+1
            t@ instr $casstk, <#>
            $casflg substr $casstk, 1, t@-1
            ifidn $casstk, <#>
                $casstk equ <>
            else
                $casstk substr $casstk, t@+1
            endif
            .endif
        endif
    endm
