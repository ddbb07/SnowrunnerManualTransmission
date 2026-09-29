import("private.action.require.impl.package")
import("private.action.require.impl.utils.get_requires")

function main()
    local requires, requires_extra = get_requires()
    print("requires count: " .. (requires and #requires or 0))
    if requires then
        for _, r in ipairs(requires) do
            print("require: " .. r)
        end
    end
    local instances = package.load_packages(requires, {requires_extra = requires_extra})
    print("loaded instances: " .. #instances)
    for _, instance in ipairs(instances) do
        print(instance:name() .. " -> " .. instance:installdir())
    end
end
