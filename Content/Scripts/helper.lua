local helper = {}
function helper.print_table(t, indent)
    indent = indent or ""
    for k, v in pairs(t) do
        local valueType = type(v)
        if valueType == "table" then
            print(indent .. tostring(k) .. ":")
            helper.print_table(v, indent .. "  ")
        elseif valueType == "string" or valueType == "number" then
            print(indent .. tostring(k) .. ": " .. tostring(v))
        elseif valueType == "userdata" then
            -- Assuming v is a sol::table which represents a C++ object
            print(indent .. tostring(k) .. ": userdata")
        end
    end
end

return helper